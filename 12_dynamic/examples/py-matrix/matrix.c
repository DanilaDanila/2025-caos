#include <Python.h>

enum {
	MATRIX_DEFAULT = 0<<0,
	MATRIX_TRANSPOSE = 1<<0,
};

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

double*
createCMatrix(PyObject *py_matr, Py_ssize_t size, int flags)
{
	double *matrix = (void*)malloc(sizeof(double) * (size * size));

	if (matrix == NULL)
		return NULL;

	Py_ssize_t matr_size = PyList_Size(py_matr);
	Py_ssize_t cpy_size = MIN(size, matr_size);

	for (Py_ssize_t i = 0; i < cpy_size; ++i)
	{
		PyObject *py_matr_row = PyList_GetItem(py_matr, i);
		Py_ssize_t row_size = PyList_Size(py_matr_row);

		Py_ssize_t row_cpy_size = MIN(size, row_size);

		for (Py_ssize_t j = 0; j < row_cpy_size; ++j)
			matrix[i * size + j] = PyFloat_AsDouble(PyList_GetItem(py_matr_row, j));

		for (Py_ssize_t j = row_cpy_size; j < size; ++j)
			matrix[i * size + j] = 0;
	}

	for (Py_ssize_t i = cpy_size; i < size; ++i)
		for (Py_ssize_t j = 0; j < size; ++j)
			matrix[i * size + j] = 0;

	if (flags & MATRIX_TRANSPOSE)
	{
		double tmp;
		for (Py_ssize_t i = 1; i < size; ++i)
			for (Py_ssize_t j = 0; j < i; ++j)
			{
				tmp = matrix[i * size + j];
				matrix[i * size + j] = matrix[j * size + i];
				matrix[j * size + i] = tmp;
			}
	}

	return matrix;
}

double*
c_dot(Py_ssize_t size, double *A, double *B_T)
{
	double *result = (void*)malloc(sizeof(double) * (size * size));

	if (result == NULL)
		return NULL;

	for (Py_ssize_t i = 0; i < size; ++i)
		for (Py_ssize_t j = 0; j < size; ++j)
		{
			result[j * size + i] = 0;
			for (Py_ssize_t k = 0; k < size; ++k)
				result[j * size + i] += A[j * size + k] * B_T[i * size + k];
		}

	return result;
}

PyObject*
cmatrixToPy(Py_ssize_t size, double *A)
{
	PyObject *py_matrix = PyList_New(size);

	for (Py_ssize_t i = 0; i < size; ++i)
	{
		PyObject *py_matrix_row = PyList_New(size);

		for (Py_ssize_t j = 0; j < size; ++j)
			PyList_SetItem(py_matrix_row, j, PyFloat_FromDouble(A[i * size + j]));

		PyList_SetItem(py_matrix, i, py_matrix_row);
	}

	return py_matrix;
}

// Py_ssize_t size, matrix A, matrix B
PyObject*
dot(PyObject *self, PyObject *args)
{
	Py_ssize_t args_count = PyTuple_Size(args);

	if (args_count != 3)
		goto err;

	Py_ssize_t size;
	PyObject *matr_A;
	PyObject *matr_B;
	if (!PyArg_ParseTuple(args, "nOO", &size, &matr_A, &matr_B))
		goto err;

	double *A = NULL, *B_T = NULL, *AB = NULL;
	A = createCMatrix(matr_A, size, MATRIX_DEFAULT);
	if (A == NULL)
		goto err;

	B_T = createCMatrix(matr_B, size, MATRIX_TRANSPOSE);
	if (B_T == NULL)
		goto err;

	AB = c_dot(size, A, B_T);
	if (AB == NULL)
		goto err;

	PyObject *matr_AB = cmatrixToPy(size, AB);

	free(AB);
	free(B_T);
	free(A);

	return matr_AB;

	err:
		if (A != NULL)
			free(A);
		if (B_T != NULL)
			free(A);
		if (AB != NULL)
			free(AB);

		Py_INCREF(Py_None);
		return Py_None;
}

PyMODINIT_FUNC PyInit_matrix()
{
	static PyMethodDef methods[] = {
		{
			.ml_name = "dot",

			.ml_meth = dot,

			.ml_flags = METH_VARARGS,

			.ml_doc = "Multiply matrix with C"
		},

		{NULL, NULL, 0, NULL},
	};

	static PyModuleDef moduleDef = {
		.m_base = PyModuleDef_HEAD_INIT,

		.m_name = "matrix",

		.m_size = -1,

		.m_methods = methods,
	};

	return PyModule_Create(&moduleDef);
}
