#include <Python.h>
#include <string>
#include <vector>
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

static PyObject* pyvicon_subjects(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    //get number of subjects
    const unsigned int sub_count = client->GetSubjectCount().SubjectCount;
    const char** subjects = new const char*[sub_count];
    
    //collect subject names into the array
    for (unsigned int i=0; i<sub_count; ++i)
        subjects[i] = ((std::string)client->GetSubjectName(i).SubjectName).c_str();
    
    
    return Py_BuildValue("[items]", subjects);
}

static PyObject* pyvicon_globalrotation(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    char* name;
    
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    
    Output_GetSegmentGlobalRotationHelical out = client->GetSegmentGlobalRotationHelical(name, name);
    //do some result checking here
    //if (out.Result == etc)
    
    //we can just pass the rotation array, let python do the rest
    return Py_BuildValue("(items)", out.Rotation);
}

static PyObject* pyvicon_globaltranslation(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    char* name;
    
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    
    Output_GetSegmentGlobalTranslation out = client->GetSegmentGlobalTranslation(name, name);
    //do some result checking here
    //if (out.Result == etc)
    
    //we can just pass the rotation array, let python do the rest
    return Py_BuildValue("(items)", out.Translation);
}

static PyObject* pyvicon_markercount(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    char* name;
    
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    
    Output_GetMarkerCount out = client->GetMarkerCount(name);
    //error checking here
    
    return Py_BuildValue("I", out.MarkerCount);
}

//declare the accessible functions
static PyMethodDef ModuleMethods[] = {
     {"connect", pyvicon_connect, METH_VARARGS, "Connect to vicon"},
     {"disconnect", pyvicon_disconnect, METH_NOARGS, "Disconnect from vicon"},
     {"isconnected", pyvicon_isconnected, METH_NOARGS, "Connection status"},
     {"subjectcount", pyvicon_subjectcount, METH_NOARGS, "Get a count of subjects"},
     {"subjectname", pyvicon_subjectname, METH_VARARGS, "Get a subject name, given an index"},
     {"subjects", pyvicon_subjects, METH_NOARGS, "Get a list of all subjects"},
     {"globalrotation", pyvicon_globalrotation, METH_VARARGS, "get global rotation of a subject"},
     {"globaltraslation", pyvicon_globaltranslation, METH_VARARGS, "get global translation of a subject"},
     {"markercount", pyvicon_markercount, METH_VARARGS, "get number of visible markers of a subject"},
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
