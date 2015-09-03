#include <Python.h>
#include <string>
#include "Client.h"

using namespace ViconDataStreamSDK;
using namespace CPP;

static PyObject* pyvicon_connect(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0); 
    char* input;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &input))
        return NULL;
    
    // run connect if not connected, error handling..?
    if (!client->IsConnected().Connected)
        client->Connect(input);
    return NULL;
}

static PyObject* pyvicon_disconnect(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    //disconnect - if connected
    if (client->IsConnected().Connected)
        client->Disconnect();
    return NULL;
}

static PyObject* pyvicon_isconnected(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    // status, as pybool
    return client->IsConnected().Connected ? Py_True : Py_False;
}

static PyObject* pyvicon_subjectcount(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    unsigned int sub_count = client->GetSubjectCount().SubjectCount;
    return Py_BuildValue("I", sub_count);
}

static PyObject* pyvicon_subjectname(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    unsigned int index;
    
    if (!PyArg_ParseTuple(args, "I", index))
        return NULL;
    
    //cast from the silly vicon string type
    std::string sub_name = (std::string)client->GetSubjectName(index).SubjectName;
    
    return Py_BuildValue("s", sub_name.c_str());
}

//declare the accessible functions
static PyMethodDef ModuleMethods[] = {
     {"connect", pyvicon_connect, METH_VARARGS, "Connect to vicon"},
     {"disconnect", pyvicon_disconnect, METH_NOARGS, "Disconnect from vicon"},
     {"isconnected", pyvicon_isconnected, METH_NOARGS, "Connection status"},
     {"subjectcount", pyvicon_subjectcount, METH_NOARGS, "Get a count of subjects"},
     {NULL, NULL, 0, NULL},
};

//DL_EXPORT(void) initpyvicon(void) {
PyMODINIT_FUNC initpyvicon(void) {
    //create the module
    PyObject* m;
    m = Py_InitModule("pyvicon", ModuleMethods);
    
    //create vicon client and add to the module - inside a capsule
    Client* vicon_client = new Client();
    PyObject* pyclient = PyCapsule_New(vicon_client, "pyvicon.client", NULL);
    PyModule_AddObject(m, "client", pyclient);
}
