// 
// Vicon python bindings
// Written for the EagleEye project at the University of South Australia
// 
// 2015-09-10
// Gwilyn Saunders
//
// Exposes a subset of the C++ Client class as Python functions.
// 

#include <Python.h>
#include <string>
#include "client.h"

using namespace ViconDataStreamSDK;
using namespace CPP;

static PyObject* ViconError;
static PyObject* pyclient;

// returns true if there is an exception to raise
static bool handleError(Result::Enum result) {
    switch (result) {
        case Result::Success:
            return false; //keep going!!
        case Result::ClientAlreadyConnected:
            PyErr_SetString(ViconError, "PyVicon: Client already connected");
            return true;
        case Result::NotConnected:
            PyErr_SetString(ViconError, "PyVicon: Client not connected");
            return true;
        case Result::ClientConnectionFailed:
            PyErr_SetString(ViconError, "PyVicon: Client connected failed");
            return true;
        case Result::InvalidHostName:
            PyErr_SetString(ViconError, "PyVicon: Invalid host name");
            return true;
        case Result::InvalidIndex:
            PyErr_SetString(ViconError, "PyVicon: Invalid index");
            return true;
        case Result::InvalidSegmentName:
            PyErr_SetString(ViconError, "PyVicon: Invalid segment name");
            return true;
        case Result::InvalidSubjectName:
            PyErr_SetString(ViconError, "PyVicon: Invalid subject name");
            return true;
        case Result::NoFrame:
            PyErr_SetString(ViconError, "PyVicon No frame - hint: if client.frame(): ... ");
            return true;
        default:
            PyErr_SetString(ViconError, "PyVicon: Unknown error - sorry!");
            return true;
    }
}

//--------------  Connect/disconnect/isConnected functions -------------

static PyObject* pyvicon_connect(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    char* input;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &input)) return NULL;
    
    // connect, ignore most results
    Output_Connect out = client->Connect(input);
    
    //true if connected, false if failed
    switch (out.Result) {
        case Result::Success:
            return Py_True;
        case Result::ClientAlreadyConnected:
            return Py_True;
        case Result::ClientConnectionFailed:
            return Py_False;
        default:
            break;
    }
    
    //raise errors for everything else
    if (handleError(out.Result)) return NULL;
    
    //catch the rest
    return Py_False;
}

static PyObject* pyvicon_disconnect(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    
    //run disconnect, return none
    client->Disconnect();
    return Py_None;
}

static PyObject* pyvicon_isconnected(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    
    // status, as pybool
    return client->IsConnected().Connected ? Py_True : Py_False;
}

//------------------------- utility functions ------------------------

static PyObject* pyvicon_version(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    Output_GetVersion out = client->GetVersion();
    return Py_BuildValue("III", out.Major, out.Minor, out.Point);
}

static PyObject* pyvicon_enablesegmentdata(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    return (client->EnableSegmentData().Result == Result::Success) ? Py_True : Py_False;
}

static PyObject* pyvicon_enablemarkerdata(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    return (client->EnableMarkerData().Result == Result::Success) ? Py_True : Py_False;
}

static PyObject* pyvicon_setstreammode(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
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
    
    //return result
    return (client->SetStreamMode(mode).Result == Result::Success) ? Py_True : Py_False;
}

//------------------------ subject getters ----------------------------

static PyObject* pyvicon_subjectcount(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    
    //get
    Output_GetSubjectCount out = client->GetSubjectCount();
    if (handleError(out.Result)) return NULL;
    
    return Py_BuildValue("I", out.SubjectCount);
}

static PyObject* pyvicon_subjectname(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    unsigned int index;
    
    if (!PyArg_ParseTuple(args, "I", &index))
        return NULL;
    
    Output_GetSubjectName out = client->GetSubjectName(index);
    if (handleError(out.Result)) return NULL;
    
    if (out.Result == Result::InvalidIndex) {
        PyErr_SetString(ViconError, "subjectName() Invalid Index");
        return NULL;
    }
    
    //cast from the silly vicon string type
    std::string sub_name = (std::string)out.SubjectName;
    return Py_BuildValue("s", sub_name.c_str());
}

static PyObject* pyvicon_subjects(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    
    //get number and handle errors
    Output_GetSubjectCount count_out = client->GetSubjectCount();
    if (handleError(count_out.Result)) return NULL;
    
    //get count and create py list
    const unsigned int sub_count = count_out.SubjectCount;
    PyObject* subjects = PyList_New(sub_count);
    
    //collect subject names into the list
    for (unsigned int i=0; i<sub_count; i++) {
        Output_GetSubjectName name_out = client->GetSubjectName(i);
        if (handleError(name_out.Result)) return NULL; //does this mem leak the subjects var?
        
        //cast and insert
        PyList_SetItem(subjects, i, Py_BuildValue("s", ((std::string)name_out.SubjectName).c_str()));
    }
    
    //hope to God the list is full
    return subjects;
}

//--------------------- rotation/translation getters ----------------------

static PyObject* pyvicon_globalrotation(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    char* name;
    
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    
    //get data, run error checking
    Output_GetSegmentGlobalRotationHelical out = client->GetSegmentGlobalRotationHelical(name, name);
    if (handleError(out.Result)) return NULL;
    
    //let python do the rest, just know it's a set of 3 doubles
    return Py_BuildValue("ddd", out.Rotation[0], out.Rotation[1], out.Rotation[2]);
}

static PyObject* pyvicon_globaltranslation(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    char* name;
    
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    
    //get data, run error checking
    Output_GetSegmentGlobalTranslation out = client->GetSegmentGlobalTranslation(name, name);
    if (handleError(out.Result)) return NULL;
    
    //magic, isn't it?
    return Py_BuildValue("ddd", out.Translation[0], out.Translation[1], out.Translation[2]);
}

//------------------------ marker, frame, other --------------------------

static PyObject* pyvicon_frame(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    return (client->GetFrame().Result == Result::Success) ? Py_True : Py_False;
}

static PyObject* pyvicon_framenumber(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    return Py_BuildValue("I", client->GetFrameNumber().FrameNumber);
}

static PyObject* pyvicon_getmarkers(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    char* name;
    
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    
    //get, errors, etc
    Output_GetMarkerCount out = client->GetMarkerCount(name);
    if (handleError(out.Result)) return NULL;
    
    PyObject* markers = PyList_New(out.MarkerCount);
    for (unsigned int i=0; i<out.MarkerCount; i++) {
        Output_GetMarkerName name_out = client->GetMarkerName(name, i);
        if (handleError(name_out.Result)) return NULL;
        
        PyList_SetItem(markers, i, Py_BuildValue("s", ((std::string)name_out.MarkerName).c_str()));
    }
    
    return markers;
}

static PyObject* pyvicon_markerstatus(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    char* name;
    
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    
    //get, errors, etc
    Output_GetMarkerCount count_out = client->GetMarkerCount(name);
    if (handleError(count_out.Result)) return NULL;
    
    unsigned int total = count_out.MarkerCount;
    unsigned int visible = 0;
    
    for (unsigned int i=0; i<total; i++) {
        Output_GetMarkerName name_out = client->GetMarkerName(name, i);
        if (handleError(name_out.Result)) return NULL;
        
        Output_GetMarkerGlobalTranslation test_out = client->GetMarkerGlobalTranslation(name, name_out.MarkerName);
        if (!test_out.Occluded) 
            visible++;
    }
    
    free(name);
    return Py_BuildValue("II", total, visible);
}

static PyObject* pyvicon_markeroccluded(PyObject* self, PyObject* args) {
    Client* client = (Client*)pyclient;
    char* subject; char* marker;
    
    if (!PyArg_ParseTuple(args, "ss", &subject, &marker))
        return NULL;
    
    Output_GetMarkerGlobalTranslation out = client->GetMarkerGlobalTranslation(subject, marker);
    //if (handleError(out.Result)) return NULL;
    
    free(subject); free(marker);
    return out.Occluded ? Py_True : Py_False;
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
     {"markersOf", pyvicon_getmarkers, METH_VARARGS, "Get all markers of a subject"},
     {"isMarkerOccluded", pyvicon_markeroccluded, METH_VARARGS, "Test if a marker is visible"},
     {"markerStatus", pyvicon_markerstatus, METH_VARARGS, "Total and visible marker counts of a subject"},
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
    pyclient = PyCapsule_New(vicon_client, "pyvicon.client", NULL);
    Py_INCREF(pyclient);
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
