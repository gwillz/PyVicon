#include <Python.h>
#include <string>
#include "Client.h"

using namespace ViconDataStreamSDK;
using namespace CPP;

static PyObject *ViconError;

//--------------  Connect/disconnect/isConnected functions -------------

static PyObject* pyvicon_connect(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0); 
    char* input;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &input))
        return NULL;
    
    // throw and error if already connected
    if (client->IsConnected().Connected) {
        PyErr_SetString(ViconError, "connect() PyVicon already connected");
        return NULL;
    }
    
    // connect, ignore most results
    //Output_Connect out = client->Connect(input);
    //if (out.Result == Result::Success)
    //    return Py_True; // they can figure out the rest
    
    return Py_True;
}

static PyObject* pyvicon_disconnect(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    //raise errors
    if (client->IsConnected().Connected) {
        PyErr_SetString(ViconError, "disconnect() PyVicon already connected");
        return NULL;
    }
    //client->Disconnect();
    return Py_None;
}

static PyObject* pyvicon_isconnected(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    // status, as pybool
    return Py_True;
}

//------------------------- utility functions ------------------------

static PyObject* pyvicon_version(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    Output_GetVersion out = client->GetVersion();
    unsigned int version[3] = {1, 5, 2};
    
    return Py_BuildValue("(I,I,I)", version);
}

static PyObject* pyvicon_enablesegmentdata(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    return Py_True;
}

static PyObject* pyvicon_enablemarkerdata(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    return Py_True;
}

static PyObject* pyvicon_setstreammode(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    int input;
    StreamMode::Enum mode;
    
    //parse the argument
    if (!PyArg_ParseTuple(args, "i", &input))
        return NULL;
    if (input < 0 && input > 2) {
        PyErr_SetString(PyExc_TypeError, "setStreamMode() takes ints (0, 1, 2)");
        return NULL;
    }
    
    //convert to the enum
    switch (input) {
        case 0:
            mode = StreamMode::ClientPull;
            break;
        case 1:
            mode = StreamMode::ClientPullPreFetch;
            break;
        default:
            mode = StreamMode::ServerPush;
    }
    
    return Py_True;
}

//------------------------ subject getters ----------------------------

static PyObject* pyvicon_subjectcount(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    return Py_BuildValue("I", 1);
}

static PyObject* pyvicon_subjectname(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    unsigned int index;
    
    if (!PyArg_ParseTuple(args, "I", &index))
        return NULL;
    
    
    return Py_BuildValue("s", "Wand");
}

static PyObject* pyvicon_subjects(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    
    //get number of subjects
    const unsigned int sub_count = 1;
    PyObject* subjects = PyList_New(0);
    
    //collect subject names into the array
    for (unsigned int i=0; i<sub_count; i++)
        PyList_Append(subjects, Py_BuildValue("s", "Wand"));
    
    return subjects;
}

//--------------------- rotation/translation getters ----------------------

static PyObject* pyvicon_globalrotation(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    char* name;
    
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    
    double rotation[3] = {1.0, 2.0, 3.0};
    
    //we can just pass the rotation array, let python do the rest
    return Py_BuildValue("(d,d,d)", rotation);
}

static PyObject* pyvicon_globaltranslation(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    char* name;
    
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    
    double translation[3] = {100.1, 200.2, 300.3};
    
    return Py_BuildValue("(d,d,d)", translation);
}

//------------------------ marker, frame, other --------------------------

static PyObject* pyvicon_frame(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    return Py_True;
}

static PyObject* pyvicon_framenumber(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    return Py_BuildValue("I", 10);
}

static PyObject* pyvicon_markercount(PyObject* self, PyObject* args) {
    Client* client = (Client*)PyCapsule_Import("pyvicon.client", 0);
    char* name;
    
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    
    return Py_BuildValue("I", 20);
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
     {"globalTranslation", pyvicon_globaltranslation, METH_VARARGS, "Get global translation of a subject"},
     {"markerCount", pyvicon_markercount, METH_VARARGS, "Get number of visible markers of a subject"},
     {"frame", pyvicon_frame, METH_NOARGS, "I don't know, a status thing?"},
     {"frameNumber", pyvicon_framenumber, METH_NOARGS, "Current frame number"},
     {"setStreamMode", pyvicon_setstreammode, METH_VARARGS, "Stream mode: Pull, PreFetch, Push"},
     {"enableSegmentData", pyvicon_enablesegmentdata, METH_NOARGS, "Enables segment data. Just always use it, I guess."},
     {"enableMarkerData", pyvicon_enablemarkerdata, METH_NOARGS, "Enables marker data. Use this for markerCount."},
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
    
    //create+add ViconError
    ViconError = PyErr_NewException("pyvicon.error", NULL, NULL);
    Py_INCREF(ViconError);
    PyModule_AddObject(m, "error", ViconError);
    
    //add StreamMode enum, not that we use anything but clientpull
    PyObject* sm1 = Py_BuildValue("i", 0);
    PyObject* sm2 = Py_BuildValue("i", 1);
    PyObject* sm3 = Py_BuildValue("i", 2);
    
    PyObject_SetAttrString(m, "SM_ClientPull", sm1);
    PyObject_SetAttrString(m, "SM_ClientPullPreFetch", sm2);
    PyObject_SetAttrString(m, "SM_ServerPush", sm3);
    
    //clean up
    Py_DECREF(sm1);
    Py_DECREF(sm2);
    Py_DECREF(sm3);
}
