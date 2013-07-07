/*
*   Copyright 2013 James Larsen
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/

#include <Python.h>
#include <xdo.h>

/* this struct is a python object holding a reference to an xdo token */
typedef struct {
  PyObject_HEAD
  xdo_t* xdo;
} xdo_tok;


/* METHOD DECLARATIONS */

/* module init method */
PyMODINIT_FUNC initxdo(void);

/* create a new xdo_tok struct */
static xdo_tok * xdo_tok_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds);

/* dealloc an existing xdo_tok struct */
static void xdo_tok_free(xdo_tok * tok);

/* describe xdo object */
static PyObject * xdo_to_string(xdo_tok * tok);

/* libxdo version method */
static PyObject * libxdo_version(PyObject *self);

/* mouse move on screen */
static PyObject * mouse_move_screen(PyObject *self, PyObject * args);

/* mouse move relative to window */
static PyObject * mouse_move_window(PyObject *self, PyObject * args);

/* mouse move relative to cur position */
static PyObject * mouse_move_relative(PyObject *self, PyObject * args);

/* send mouse down for button to window */
static PyObject * mouse_down(PyObject *self, PyObject * args);

/* send mouse up for button to window */
static PyObject * mouse_up(PyObject *self, PyObject * args);

/* get the current mouse location */
static PyObject * mouse_location(PyObject *self, PyObject *args);

/* send a mouse click */
static PyObject * mouse_click(PyObject *self, PyObject *args);

/* type out chars */
static PyObject * type(PyObject *self, PyObject *args, PyObject *kwargs);

/* Send any combination of X11 KeySym names separated by '+' */
static PyObject * key_sequence(PyObject *self, PyObject *args, PyObject *kwargs);

/* Send any down combination of X11 KeySym names separated by '+' */
static PyObject * key_sequence_down(PyObject *self, PyObject *args, PyObject *kwargs);

/* Send any up combination of X11 KeySym names separated by '+' */
static PyObject * key_sequence_up(PyObject *self, PyObject *args, PyObject *kwargs);

/* Move the top left corner of a window to x,y */
static PyObject * window_move(PyObject *self, PyObject *args);

/* Resize a window to x,y */
static PyObject * window_resize(PyObject *self, PyObject *args);

/* Focus a window */
static PyObject * window_focus(PyObject *self, PyObject *args);

/* Raise a window */
static PyObject * window_raise(PyObject *self, PyObject *args);

/* Activate a window */
static PyObject * window_activate(PyObject *self, PyObject *args);


static PyMethodDef xdo_tok_methods[] = {
    {NULL}
};

static PyTypeObject xdo_tok_type = {
    PyObject_HEAD_INIT(NULL)
    .tp_name = "xdo.xdo",
    .tp_basicsize = sizeof(xdo_tok),
    .tp_dealloc = (destructor)xdo_tok_free, 
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "xdo token",
    .tp_new = xdo_tok_alloc,
    .tp_str = (reprfunc)xdo_to_string,
    .tp_methods = xdo_tok_methods
};


static PyMethodDef module_methods[] = {
    {"version",  (PyCFunction)libxdo_version, METH_NOARGS, "Returns the libxdo version"},
    {"mouse_move",  (PyCFunction)mouse_move_screen, METH_VARARGS, "Move the mouse relative to a screen"},
    {"mouse_move_window",  (PyCFunction)mouse_move_window, METH_VARARGS, "Move the mouse relative to a window"},
    {"mouse_move_relative",  (PyCFunction)mouse_move_relative, METH_VARARGS, "Move the mouse relative to current position"},
    {"mouse_down",  (PyCFunction)mouse_down, METH_VARARGS, "Send a mouse down from a button to a window"},
    {"mouse_up",  (PyCFunction)mouse_up, METH_VARARGS, "Send a mouse up from a button to a window"},
    {"mouse_location",  (PyCFunction)mouse_location, METH_VARARGS, "Get the current mouse x,y, and screen"},
    {"mouse_click",  (PyCFunction)mouse_click, METH_VARARGS, "Send a mouse click to a window at the current mouse location"},
    {"type",  (PyCFunction)type, METH_VARARGS|METH_KEYWORDS, "Type a string out to a window"},
    {"key_sequence",  (PyCFunction)key_sequence, METH_VARARGS|METH_KEYWORDS, "Send any combination of X11 KeySym names separated by a +"},
    {"key_sequence_down",  (PyCFunction)key_sequence_down, METH_VARARGS|METH_KEYWORDS, "Send any down combination of X11 KeySym names separated by a +"},
    {"key_sequence_up",  (PyCFunction)key_sequence_up, METH_VARARGS|METH_KEYWORDS, "Send any up combination of X11 KeySym names separated by a +"},
    {"window_move",  (PyCFunction)window_move, METH_VARARGS, "Move the top left corner of a window to x,y"},
    {"window_resize",  (PyCFunction)window_resize, METH_VARARGS, "Resize a window to x,y"},
    {"window_focus",  (PyCFunction)window_focus, METH_VARARGS, "Focus a window"},
    {"window_raise",  (PyCFunction)window_raise, METH_VARARGS, "Raise a window"},
    {"window_activate",  (PyCFunction)window_activate, METH_VARARGS, "Activate a window"},
    {NULL}
};


/* module init method */
PyMODINIT_FUNC initxdo(void) {
    PyObject* m;

    if (PyType_Ready(&xdo_tok_type) < 0)
        return;

    m = Py_InitModule3("xdo", module_methods, "Python binding for libxdo2");

    Py_INCREF(&xdo_tok_type);
    PyModule_AddObject(m, "xdo", (PyObject *)&xdo_tok_type);
}


/* Activate a window */
static PyObject * window_activate(PyObject *self, PyObject *args){
	xdo_tok * tok;
	int window;
	
	/* parse the args */
	if(!PyArg_ParseTuple(args,"O!i",&xdo_tok_type,&tok,&window)) {goto err1;}
	
	int res =  xdo_window_actiavte(tok->xdo,(Window)window);

	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}


/* Focus a window */
static PyObject * window_raise(PyObject *self, PyObject *args){
	xdo_tok * tok;
	int window;
	
	/* parse the args */
	if(!PyArg_ParseTuple(args,"O!i",&xdo_tok_type,&tok,&window)) {goto err1;}
	
	int res =  xdo_window_raise(tok->xdo,(Window)window);

	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}


/* Focus a window */
static PyObject * window_focus(PyObject *self, PyObject *args){
	xdo_tok * tok;
	int window;
	
	/* parse the args */
	if(!PyArg_ParseTuple(args,"O!i",&xdo_tok_type,&tok,&window)) {goto err1;}
	
	int res =  xdo_window_focus(tok->xdo,(Window)window);

	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}

/* Resize a window to x,y */
static PyObject * window_resize(PyObject *self, PyObject *args){
	xdo_tok * tok;
	int x,y,window;
	
	/* parse the args */
	if(!PyArg_ParseTuple(args,"O!iii",&xdo_tok_type,&tok,&x,&y,&window)) {goto err1;}
	
	int res = xdo_window_setsize(tok->xdo,(Window)window,x,y,0);

	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}


/* Move the top left corner of a window to x,y */
static PyObject * window_move(PyObject *self, PyObject *args){
	xdo_tok * tok;
	int x,y,window;
	
	/* parse the args */
	if(!PyArg_ParseTuple(args,"O!iii",&xdo_tok_type,&tok,&x,&y,&window)) {goto err1;}
	
	int res = xdo_window_move(tok->xdo,(Window)window,x,y);

	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}


/* Send any combination of X11 KeySym names separated by '+' */
static PyObject * key_sequence_up(PyObject *self, PyObject *args, PyObject *kwargs){
	static char *kwlist[] = {"xdo", "string", "delay", "window", NULL};

	xdo_tok * tok;
	const char *string;
	int delay = 12000;
	int window;

	if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O!s|ii", kwlist, &xdo_tok_type, &tok, &string, &delay, &window)) {goto err1;}
	
	int res = xdo_keysequence_up(tok->xdo, (Window)window, string, delay);
	
	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}


/* Send any combination of X11 KeySym names separated by '+' */
static PyObject * key_sequence_down(PyObject *self, PyObject *args, PyObject *kwargs){
	static char *kwlist[] = {"xdo", "string", "delay", "window", NULL};

	xdo_tok * tok;
	const char *string;
	int delay = 12000;
	int window;

	if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O!s|ii", kwlist, &xdo_tok_type, &tok, &string, &delay, &window)) {goto err1;}
	
	int res = xdo_keysequence_down(tok->xdo, (Window)window, string, delay);
	
	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}


/* Send any combination of X11 KeySym names separated by '+' */
static PyObject * key_sequence(PyObject *self, PyObject *args, PyObject *kwargs){
	static char *kwlist[] = {"xdo", "string", "delay", "window", NULL};

	xdo_tok * tok;
	const char *string;
	int delay = 12000;
	int window;

	if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O!s|ii", kwlist, &xdo_tok_type, &tok, &string, &delay, &window)) {goto err1;}
	
	int res = xdo_keysequence(tok->xdo, (Window)window, string, delay);
	
	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}


/* type out chars */
static PyObject * type(PyObject *self, PyObject *args, PyObject *kwargs){
	static char *kwlist[] = {"xdo", "string", "delay", "window", NULL};

	xdo_tok * tok;
	const char *string;
	int delay = 12000;
	int window;

	if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O!s|ii", kwlist, &xdo_tok_type, &tok, &string, &delay, &window)) {goto err1;}
	
	int res = xdo_type(tok->xdo,(Window)window,string,delay);
	
	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}

/* send a mouse click */
static PyObject * mouse_click(PyObject *self, PyObject *args){
	xdo_tok * tok;
	int button,window;
	
	/* parse the args */
	if(!PyArg_ParseTuple(args,"O!ii",&xdo_tok_type,&tok,&button,&window)) {goto err1;}
	
	int res = xdo_click(tok->xdo,(Window)window,button);

	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}

/* get the current mouse location */
static PyObject * mouse_location(PyObject *self, PyObject *args){
	xdo_tok * tok;
	int x,y,screen;

	/* parse the args */
	if(!PyArg_ParseTuple(args,"O!",&xdo_tok_type,&tok)) {goto err1;}

	xdo_mouselocation(tok,&x,&y,&screen);

	PyObject * res = Py_BuildValue("(iii)",x,y,screen);

	return res;
	
	err1:
	return NULL;

}



/* send mouse down for button to window */
static PyObject * mouse_down(PyObject *self, PyObject * args){
	xdo_tok * tok;
	int button,window;
	
	/* parse the args */
	if(!PyArg_ParseTuple(args,"O!ii",&xdo_tok_type,&tok,&button,&window)) {goto err1;}
	
	int res = xdo_mousedown(tok->xdo,(Window)window,button);

	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}


/* send mouse up for button to window */
static PyObject * mouse_up(PyObject *self, PyObject * args){
	xdo_tok * tok;
	int button,window;
	
	/* parse the args */
	if(!PyArg_ParseTuple(args,"O!ii",&xdo_tok_type,&tok,&button,&window)) {goto err1;}
	
	int res = xdo_mouseup(tok->xdo,(Window)window,button);

	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}


/* mouse move relative to cur position */
static PyObject * mouse_move_relative(PyObject *self, PyObject * args){
	xdo_tok * tok;
	int x,y;
	
	/* parse the args */
	if(!PyArg_ParseTuple(args,"O!ii",&xdo_tok_type,&tok,&x,&y)) {goto err1;}
	
	int res = xdo_mousemove_relative(tok->xdo,x,y);

	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}


/* mouse move relative to window */
static PyObject * mouse_move_window(PyObject *self, PyObject * args){
	xdo_tok * tok;
	int x,y,window;
	
	/* parse the args */
	if(!PyArg_ParseTuple(args,"O!iii",&xdo_tok_type,&tok,&x,&y,&window)) {goto err1;}
	
	int res = xdo_mousemove_relative_to_window(tok->xdo,(Window)window,x,y);

	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}


/* mouse move on screen */
static PyObject * mouse_move_screen(PyObject *self, PyObject * args){
	xdo_tok * tok;
	int x,y,screen;
	
	/* parse the args */
	if(!PyArg_ParseTuple(args,"O!iii",&xdo_tok_type,&tok,&x,&y,&screen)) {goto err1;}
	
	int res = xdo_mousemove(tok->xdo,x,y,screen);

	return PyInt_FromLong((long)res);

	err1:
	return NULL;
}


/* libxdo version method */
static PyObject * libxdo_version(PyObject *self){
	const char * version = xdo_version();
	return PyString_FromString(version);
}



/* create a new xdo_tok struct */
static xdo_tok * xdo_tok_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds){
	xdo_tok * tok = NULL;
	const char * display = NULL;
	
	if(!PyArg_ParseTuple(args, "|s", &display)) {goto err1;}

	tok = (xdo_tok *)type->tp_alloc(type,0);
	if(tok == NULL) {goto err1;}

	tok->xdo = xdo_new(display);
	if(tok->xdo == NULL) {goto err2;}

    return (PyObject *)tok;

	err2:
	tok->ob_type->tp_free((PyObject*)tok);

	err1:
	return NULL;	
}


/*dealloc an existing xdo_tok struct */
static void xdo_tok_free(xdo_tok * tok){
	if(tok->xdo != NULL) {xdo_free(tok->xdo);}
    tok->ob_type->tp_free((PyObject*)tok);
}


/* describe xdo object */
static PyObject * xdo_to_string(xdo_tok * tok){
	return PyString_FromFormat("xdo token (%p)", tok->xdo);
}
