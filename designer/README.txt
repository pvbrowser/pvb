################################################################################
# If you want to use Qt Designer for designing your masks,                     #
# you must install the plugins for the custom widgets.                         #
#                                                                              #
# Please copy the files within "plugins" to your Qt Designer plugins directory #
# On openSUSE this directory is:                                               #
# /usr/lib/qt4/plugins/designer   or   /usr/lib64/qt4/plugins/designer         #
# Or try to find the directory with the following command                      #
# find /usr -name designer | grep plugins | grep qt                            #
################################################################################

### List of Qt Designer widgets supported by pvbrowser #########################

### Qt widgets #################################################################
QPushButton           # normal button
QLabel                # display text
QLineEdit             # input text
QComboBox             # select choices from box
QLCDNumber            # display LCD numbers
QRadioButton          # radio button (grouped by QGroupBox)
QCheckBox             # check box
QSlider               # slider
QTableWidget          # table
QGLWidget             # display openGL graphics
QGroupBox             # group objects
QListBox              # list items
QSpinBox              # spin box
QDial                 # dial
QProgressBar          # progress bar
QTextBrowser          # browser html pages
QTextEdit             # text editor
QListWidget           # list objects

### Qwt widgets ################################################################
QwtPlot               # used for displaying xy-graphics 
QwtThermo             #
QwtKnob               #
QwtCounter            #
QwtWheel              #
QwtSlider             #
QwtCompass            #

### pvbrowser widgets ##########################################################
PvbDraw               # used for graphics funktions and SVG
PvbImage              # used for displaying bitmap graphics
PvbVTK                # used for VTK rendering
PvbOpengl             # used for OpenGL output
PvbIconview           # used instead of no longer existing QIconView

