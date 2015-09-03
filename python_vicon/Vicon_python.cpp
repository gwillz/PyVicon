#include <Python.h>
#include <string>
#include "Client.h"

using namespace ViconDataStreamSDK;
using namespace CPP;

//--------------  Connect/disconnect/isConnected functions -------------

static PyObject* pyvicon_connect(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0); 
    char* input;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &input))
        return NULL;
    
    // return true if already connected
    if (client->IsConnected().Connected)
        return Py_True;
    
    // connect, ignore most results
    Output_Connect out = client->Connect(input);
    if (out.Result == Result::Success)
        return Py_True; // they can figure out the rest
    
    return Py_False;
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

//------------------------- utility functions ------------------------

static PyObject* pyvicon_version(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    Output_GetVersion out = client->GetVersion();
    unsigned int version[3] = {out.Major, out.Minor, out.Point};
    
    return Py_BuildValue("(items)", version);
}

static PyObject* pyvicon_enablesegmentdata(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    return NULL; //STUB
}

static PyObject* pyvicon_setstreammode(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    return NULL; //STUB
}

//------------------------ subject getters ----------------------------

static PyObject* pyvicon_subjectcount(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    unsigned int sub_count = client->GetSubjectCount().SubjectCount;
    return Py_BuildValue("I", sub_count);
}

static PyObject* pyvicon_subjectname(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    unsigned int index;
    
    if (!PyArg_ParseTuple(args, "I", &index))
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

//--------------------- rotation/translation getters ----------------------

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

//------------------------ marker, frame, other --------------------------

static PyObject* pyvicon_frame(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    return NULL; //STUB
}

static PyObject* pyvicon_framenumber(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    return NULL; //STUB
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

//------------------------- aaaaand the rest ----------------------------

//declare the accessible functions
static PyMethodDef ModuleMethods[] = {
     {"connect", pyvicon_connect, METH_VARARGS, "Connect to vicon"},
     {"disconnect", pyvicon_disconnect, METH_NOARGS, "Disconnect from vicon"},
     {"isConnected", pyvicon_isconnected, METH_NOARGS, "Connection status"},
     {"subjectCount", pyvicon_subjectcount, METH_NOARGS, "Get a count of subjects"},
     {"subjectName", pyvicon_subjectname, METH_VARARGS, "Get a subject name, given an index"},
     {"subjects", pyvicon_subjects, METH_NOARGS, "Get a list of all subjects"},
     {"globalRotation", pyvicon_globalrotation, METH_VARARGS, "Get global rotation of a subject"},
     {"globalTraslation", pyvicon_globaltranslation, METH_VARARGS, "Get global translation of a subject"},
     {"markerCount", pyvicon_markercount, METH_VARARGS, "Get number of visible markers of a subject"},
     {"frame", pyvicon_frame, METH_NOARGS, "I don't know, a status thing?"},
     {"frameNumber", pyvicon_framenumber, METH_NOARGS, "Current frame number"},
     {"setStreamMode", pyvicon_setstreammode, METH_VARARGS, "Stream mode: Pull, PreFetch, Push"},
     {"enableSegmentData", pyvicon_enablesegmentdata, METH_NOARGS, "Enables segment data. Just always use it, I guess."},
     {"version", pyvicon_version, METH_NOARGS, "Vicon system version"},
     {NULL, NULL, 0, NULL},
};

PyMODINIT_FUNC initpyvicon(void) {
    //create the module
    PyObject* m;
    m = Py_InitModule("pyvicon", ModuleMethods);
    
    //create vicon client and add to the module - inside a capsule
    Client* vicon_client = new Client();
    PyObject* pyclient = PyCapsule_New(vicon_client, "pyvicon.client", NULL);
    PyModule_AddObject(m, "client", pyclient);
}
