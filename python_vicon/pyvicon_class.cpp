#include <Python.h>
#include <string>
#include "Client.h"

using namespace ViconDataStreamSDK;
using namespace CPP;

static PyObject* ViconError;

static PyObject* pyvicon_newclient(PyObject* self, PyObject* args) {
    //create the client, wrap it, return it
    return PyCapsule_New(new Client(), NULL, NULL);
}

//--------------  Connect/disconnect/isConnected functions -------------

static PyObject* pyvicon_connect(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    char* address;
    
    //parse
    if (!PyArg_ParseTuple(args, "Os", &capsule, &address)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    // throw and error if already connected
    // although this probably comes out in the Result..
    if (client->IsConnected().Connected) {
        PyErr_SetString(ViconError, "connect() PyVicon already connected");
        return NULL;
    }
    
    // connect, ignore most results
    Output_Connect out = client->Connect(address);
    if (out.Result == Result::Success)
        return Py_True; // they can figure out the rest
    
    return Py_False;
}

static PyObject* pyvicon_disconnect(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    
    //parse
    if (!PyArg_ParseTuple(args, "O", &capsule)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    //raise errors
    if (client->IsConnected().Connected) {
        PyErr_SetString(ViconError, "disconnect() PyVicon already connected");
        return NULL;
    }
    
    //run disconnect, return none
    client->Disconnect();
    return Py_None;
}

static PyObject* pyvicon_isconnected(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    
    //parse
    if (!PyArg_ParseTuple(args, "O", &capsule)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    // status, as pybool
    return client->IsConnected().Connected ? Py_True : Py_False;
}

//------------------------- utility functions ------------------------

static PyObject* pyvicon_version(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    
    //parse
    if (!PyArg_ParseTuple(args, "O", &capsule)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    //get
    Output_GetVersion out = client->GetVersion();
    unsigned int version[3] = {out.Major, out.Minor, out.Point};
    
    return Py_BuildValue("(I,I,I)", version);
}

static PyObject* pyvicon_enablesegmentdata(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    
    //parse
    if (!PyArg_ParseTuple(args, "O", &capsule)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    //get
    return (client->EnableSegmentData().Result == Result::Success) ? Py_True : Py_False;
}

static PyObject* pyvicon_enablemarkerdata(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    
    //parse
    if (!PyArg_ParseTuple(args, "O", &capsule)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    //get
    return (client->EnableMarkerData().Result == Result::Success) ? Py_True : Py_False;
}

static PyObject* pyvicon_setstreammode(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    StreamMode::Enum mode; int input;
    
    //parse
    if (!PyArg_ParseTuple(args, "Oi", &capsule, &input)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    //test enum support
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
    //inputs
    PyObject* capsule;
    
    if (!PyArg_ParseTuple(args, "O", &capsule)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    return Py_BuildValue("I", client->GetSubjectCount().SubjectCount);
}

static PyObject* pyvicon_subjectname(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    unsigned int index;
    
    //parse
    if (!PyArg_ParseTuple(args, "OI", &capsule, &index)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    //get and test result
    Output_GetSubjectName out = client->GetSubjectName(index);
    if (out.Result == Result::InvalidIndex) {
        PyErr_SetString(ViconError, "subjectName() Invalid Index");
        return NULL;
    }
    
    //cast from the silly vicon string type
    std::string sub_name = (std::string)out.SubjectName;
    return Py_BuildValue("s", sub_name.c_str());
}

static PyObject* pyvicon_subjects(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    
    //parse
    if (!PyArg_ParseTuple(args, "O", &capsule)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    //get number of subjects
    const unsigned int sub_count = client->GetSubjectCount().SubjectCount;
    PyObject* subjects = PyList_New(0);
    
    //collect subject names into the list (it's expandable!)
    for (unsigned int i=0; i<sub_count; i++) {
        PyList_Append(subjects, Py_BuildValue("s", 
                ((std::string) client->GetSubjectName(i).SubjectName).c_str()));
    }
    return subjects;
}

//--------------------- rotation/translation getters ----------------------

static PyObject* pyvicon_globalrotation(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    char* name;
    
    //parse
    if (!PyArg_ParseTuple(args, "Os", &capsule, &name)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    //check for segment data
    if (client->IsSegmentDataEnabled().Enabled) {
        PyErr_SetString(ViconError, "globalRotation() SegmentData not enabled");
        return NULL;
    }
    
    //get data, run error checking
    Output_GetSegmentGlobalRotationHelical out = client->GetSegmentGlobalRotationHelical(name, name);
    switch (out.Result) {
        case Result::Success:
            break; //keep going!!
        case Result::InvalidSegmentName:
            PyErr_SetString(ViconError, "globalRotation() Invalid Segment Name");
            return NULL;
        case Result::InvalidSubjectName:
            PyErr_SetString(ViconError, "globalRotation() Invalid Subject Name");
            return NULL;
        default:
            PyErr_SetString(ViconError, "globalRotation() Unknown Error - sorry!");
            return NULL;
    }
    
    //let python do the rest, just know it's a set of 3 doubles
    return Py_BuildValue("(d,d,d)", out.Rotation);
}

static PyObject* pyvicon_globaltranslation(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    char* name;
    
    //parse
    if (!PyArg_ParseTuple(args, "Os", &capsule, &name)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    //check for segment data
    if (client->IsSegmentDataEnabled().Enabled) {
        PyErr_SetString(ViconError, "globalTranslation() SegmentData not enabled");
        return NULL;
    }
    
    //get data, run error checking
    Output_GetSegmentGlobalTranslation out = client->GetSegmentGlobalTranslation(name, name);
    switch (out.Result) {
        case Result::Success:
            break; //keep going!!
        case Result::InvalidSegmentName:
            PyErr_SetString(ViconError, "globalTranslation() Invalid Segment Name");
            return NULL;
        case Result::InvalidSubjectName:
            PyErr_SetString(ViconError, "globalTranslation() Invalid Subject Name");
            return NULL;
        default:
            PyErr_SetString(ViconError, "globalTranslation() Unknown Error - sorry!");
            return NULL;
    }
    
    //magic, isn't it?
    return Py_BuildValue("(d,d,d)", out.Translation);
}

//------------------------ marker, frame, other --------------------------

static PyObject* pyvicon_frame(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    
    //parse
    if (!PyArg_ParseTuple(args, "O", &capsule)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    return (client->GetFrame().Result == Result::Success) ? Py_True : Py_False;
}

static PyObject* pyvicon_framenumber(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    
    //parse
    if (!PyArg_ParseTuple(args, "O", &capsule)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    //get
    return Py_BuildValue("I", client->GetFrameNumber().FrameNumber);
}

static PyObject* pyvicon_markercount(PyObject* self, PyObject* args) {
    //inputs
    PyObject* capsule;
    char* name;
    
    //parse
    if (!PyArg_ParseTuple(args, "Os", &capsule, &name)) return NULL;
    Client* client = (Client*)PyCapsule_GetPointer(capsule, NULL);
    
    //check for marker data
    if (client->IsMarkerDataEnabled().Enabled) {
        PyErr_SetString(ViconError, "markerCount() MarkerData not enabled");
        return NULL;
    }
    
    //get
    return Py_BuildValue("I", client->GetMarkerCount(name).MarkerCount);
}

//------------------------- aaaaand the rest ----------------------------

//declare the accessible functions
static PyMethodDef ModuleMethods[] = {
     {"new_client", pyvicon_newclient, METH_VARARGS, "Create a client object"},
     {"connect", pyvicon_connect, METH_VARARGS, "Connect to vicon"},
     {"disconnect", pyvicon_disconnect, METH_VARARGS, "Disconnect from vicon"},
     {"isConnected", pyvicon_isconnected, METH_VARARGS, "Connection status"},
     {"subjectCount", pyvicon_subjectcount, METH_VARARGS, "Get a count of subjects"},
     {"subjectName", pyvicon_subjectname, METH_VARARGS, "Get a subject name, given an index"},
     {"subjects", pyvicon_subjects, METH_VARARGS, "Get a list of all subjects"},
     {"globalRotation", pyvicon_globalrotation, METH_VARARGS, "Get global rotation of a subject"},
     {"globalTranslation", pyvicon_globaltranslation, METH_VARARGS, "Get global translation of a subject"},
     {"markerCount", pyvicon_markercount, METH_VARARGS, "Get number of visible markers of a subject"},
     {"frame", pyvicon_frame, METH_VARARGS, "I don't know, a status thing?"},
     {"frameNumber", pyvicon_framenumber, METH_VARARGS, "Current frame number"},
     {"setStreamMode", pyvicon_setstreammode, METH_VARARGS, "Stream mode: Pull, PreFetch, Push"},
     {"enableSegmentData", pyvicon_enablesegmentdata, METH_VARARGS, "Enables segment data. Just always use it, I guess."},
     {"enableMarkerData", pyvicon_enablemarkerdata, METH_VARARGS, "Enables marker data. Use this for markerCount."},
     {"version", pyvicon_version, METH_VARARGS, "Vicon system version"},
     {NULL, NULL, 0, NULL},
};

PyMODINIT_FUNC initpyvicon(void) {
    //create the module
    PyObject* m;
    m = Py_InitModule("pyvicon", ModuleMethods);
    
    //create+add ViconError
    ViconError = PyErr_NewException("pyvicon.error", NULL, NULL);
    Py_INCREF(ViconError);
    PyModule_AddObject(m, "error", ViconError);
}
