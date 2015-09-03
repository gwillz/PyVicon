#include <Python.h>
#include "Client.h"

//PyMethodDef* pyvicon_methods = PyMethodDef[4];
//pyvicon_methods[0].ml_name = "";

//static Client vicon_client;

static PyObject* pyvicon_hello(PyObject *self, PyObject *args) {
    char * input;
    char * result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &input)) {
        return NULL;
    }
    
    // run the actual function
    result = "world";
    
    // build the resulting string into a Python object.
    ret = PyString_FromString(result);
    free(result);
    
    return ret;
}

static PyMethodDef HelloMethods[] = {
 { "hello", pyvicon_hello, METH_VARARGS, "Say hello" },
 { NULL, NULL, 0, NULL }
};

DL_EXPORT(void) inithello(void)
{
  Py_InitModule("hello", HelloMethods);
}