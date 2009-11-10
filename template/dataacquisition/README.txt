You may use this template to implement your own data acquisition.
Within the client subdirectory you can see an example daemon from which you can start.
Within the pvs subdirectory there is a small pvserver that works with the client.

The template shows howto interface with a shared memory and a mailbox
according to the "pvbrowser principle".
http://pvbrowser.de/pvbrowser/index.php?menu=4&topic=4

Using the template you may add your own custom data acquisition if your interface is not already supported by pvbrowser.
The interface uses strings for variable names and values.
Thus it is easy to write pvserver's that define the variable names for example in the toolTip[] property of the widgets.
Within pvserver you use the class rlDataAcquisition from rllib.

You can also interface a technological simulation model using this template.
In each cycle of your model you could update the shared memory with values.
Thus pvserver can monitor them.
Using the mailbox thread you can send inputs like setpoints for example to the model.

