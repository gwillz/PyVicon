#include <Python.h>
#include "Client.h"
 
static ViconDataStreamSDK::CPP::Client vicon_client;

static PyObject* pyvicon_connect(PyObject* self, PyObject* args) {
    //PyObject* ret;
    char* input;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &input)) {
        return NULL;
    }
    
    vicon_client.Connect(input);
    return NULL;
}

static PyObject* pyvicon_disconnect(PyObject* self, PyObject* args) {
    ViconDataStreamSDK::CPP::Output_IsConnected stat = vicon_client.IsConnected();
    if (stat.Connected)
        vicon_client.Disconnect();
    return Py_None;
}

static PyObject* pyvicon_isconnected(PyObject* self, PyObject* args) {
    ViconDataStreamSDK::CPP::Output_IsConnected stat = vicon_client.IsConnected();
    return stat.Connected ? Py_True : Py_False;
}

static PyMethodDef ModuleMethods[] =
{
     {"connect", pyvicon_connect, METH_VARARGS, "Connect to vicon"},
     {"disconnect", pyvicon_disconnect, METH_NOARGS, "Disconnect from vicon"},
     {"isconnected", pyvicon_isconnected, METH_NOARGS, "Connection status"},
     {NULL, NULL, 0, NULL},
};

//DL_EXPORT(void) initpyvicon(void) {
PyMODINIT_FUNC initpyvicon(void) {
     (void) Py_InitModule("pyvicon", ModuleMethods);
}
