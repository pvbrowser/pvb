### pvbrowser slots written in python ################# 
### begin of generated area, do not edit ##############
import pv, rllib

class mask1:

  p = pv.PARAM()
  # our mask contains the following objects
  ID_MAIN_WIDGET = 0
  obj1 = 1
  ID_END_OF_WIDGETS = 2

  toolTip = [
    '',
    '',
    '' ]

  whatsThis = [
    '',
    '',
    '' ]

  widgetType = [
    '',
    'TQPushButton',
    '' ]

  ### end of generated area, do not edit ##############

  I = 0

  def slotInit(self, s):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotNullEvent(self, s):
    self.p.s = self.p.os = s # set socket must be the first command
    ret = pv.pvPrintf(self.p,self.obj1,'hello'+str(self.I))
    self.I = self.I + 1
    return 0

  def slotButtonEvent(self, s, id):
    self.p.s = self.p.os = s # set socket must be the first command
    if id == self.obj1:
      self.I = 0
      ret = pv.pvPrintf(self.p,self.obj1,'reset'+str(self.I))
      print 'reset I = 0'
    return 0

  def slotButtonPressedEvent(self, s, id):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotButtonReleasedEvent(self, s, id):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotTextEvent(self, s, id, text):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotSliderEvent(self, s, id, val):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotCheckboxEvent(self, s, id, text):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotRadioButtonEvent(self, s, id, text):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotGlInitializeEvent(self, s, id):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotGlPaintEvent(self, s, id):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotGlResizeEvent(self, s, id, width, height):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotGlIdleEvent(self, s, id):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotTabEvent(self, s, id, val):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotTableTextEvent(self, s, id, x, y, text):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotTableClickedEvent(self, s, id, x, y, button):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotSelectionEvent(self, s, id, val, text):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotClipboardEvent(self, s, id, val):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotRightMouseEvent(self, s, id, text):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotKeyboardEvent(self, s, id, val, modifier):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotMouseMovedEvent(self, s, id, x, y):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotMousePressedEvent(self, s, id, x, y):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotMouseReleasedEvent(self, s, id, x, y):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotMouseOverEvent(self, s, id, enter):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0

  def slotUserEvent(self, s, id, text):
    self.p.s = self.p.os = s # set socket must be the first command
    return 0
