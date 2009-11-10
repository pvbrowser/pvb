#////////////////////////////////////////////////////////////////////////////
#//
#// show_periodic for ProcessViewServer created: Fri Jul 18 07:39:21 2003
#//
#////////////////////////////////////////////////////////////////////////////
import string

#// _begin_of_generated_area_ (do not edit -> use ui2pvc) -------------------

class periodic:
  # our mask contains the following objects
  ID_MAIN_WIDGET = 0
  PushButtonBack = 1
  TabWidget2 = 2
  tab1 = 3
  PushButton2 = 4
  TextLabel1 = 5
  TextLabel2 = 6
  ListBox1 = 7
  LCDNumber1 = 8
  ProgressBar1 = 9
  SpinBox1 = 10
  Dial1 = 11
  Line1 = 12
  Line2 = 13
  PushButton1 = 14
  Table1 = 15
  GroupBox1 = 16
  PushButton4 = 17
  PushButton5 = 18
  PushButton6 = 19
  PushButton7 = 20
  tab2 = 21
  ButtonGroup1 = 22
  RadioButton3 = 23
  RadioButton2 = 24
  RadioButton1 = 25
  ButtonGroup2 = 26
  CheckBox3 = 27
  CheckBox2 = 28
  CheckBox1 = 29
  PushButtonRequestMultiLineEdit = 30
  MultiLineEdit1 = 31
  tab3 = 32
  TextLabel3 = 33
  ComboBox1 = 34
  TextLabel4 = 35
  TextLabel5 = 36
  ComboBox2 = 37
  Slider1 = 38
  TextLabel6 = 39
  Slider2 = 40
  PushButtonBackward = 41
  PushButtonForward = 42
  TextBrowser1 = 43
  PushButtonHome = 44
  LineEdit1 = 45
  tab4 = 46
  Draw1 = 47
  TextLabel2_2 = 48
  TextLabel1_2 = 49
  Frame3 = 50
  Image1 = 51
  PixmapLabel1 = 52
  tab5 = 53
  IconView1 = 54
  PushButtonRequestListView = 55
  ListView1 = 56
  tab6 = 57
  VtkTclWidget1 = 58
  VtkSlider1 = 59
  VtkSlider2 = 60
  ID_END_OF_WIDGETS = 61

  def generated_defineMask(self,pvs,p):
    w     = pvs.new_int(0)
    h     = pvs.new_int(0)
    depth = pvs.new_int(0)
    ret = pvs.pvStartDefinition(p,self.ID_END_OF_WIDGETS)

    ret = pvs.pvQPushButton(p,self.PushButtonBack,0)
    ret = pvs.pvSetGeometry(p,self.PushButtonBack,10,480,111,40)
    ret = pvs.pvSetText(p,self.PushButtonBack,"Back")

    ret = pvs.pvQTabWidget(p,self.TabWidget2,0)
    ret = pvs.pvSetGeometry(p,self.TabWidget2,10,10,680,460)

    ret = pvs.pvQWidget(p,self.tab1,self.TabWidget2)
    ret = pvs.pvAddTab(p,self.TabWidget2,self.tab1,"Labels + Buttons + More")

    ret = pvs.pvQPushButton(p,self.PushButton2,self.tab1)
    ret = pvs.pvSetGeometry(p,self.PushButton2,8,55,101,31)
    ret = pvs.pvSetText(p,self.PushButton2,"PushButton2")

    ret = pvs.pvQLabel(p,self.TextLabel1,self.tab1)
    ret = pvs.pvSetGeometry(p,self.TextLabel1,128,15,101,31)
    ret = pvs.pvSetText(p,self.TextLabel1,"TextLabel1")

    ret = pvs.pvQLabel(p,self.TextLabel2,self.tab1)
    ret = pvs.pvSetGeometry(p,self.TextLabel2,128,55,101,31)
    ret = pvs.pvSetText(p,self.TextLabel2,"TextLabel2")

    ret = pvs.pvQListBox(p,self.ListBox1,self.tab1)
    ret = pvs.pvSetGeometry(p,self.ListBox1,400,10,140,140)

    ret = pvs.pvQLCDNumber(p,self.LCDNumber1,self.tab1,2,pvs.Outline,pvs.Dec)
    ret = pvs.pvSetGeometry(p,self.LCDNumber1,550,170,111,41)

    ret = pvs.pvQProgressBar(p,self.ProgressBar1,self.tab1,50)
    ret = pvs.pvSetGeometry(p,self.ProgressBar1,548,265,121,31)

    ret = pvs.pvQSpinBox(p,self.SpinBox1,self.tab1,3,112,1)
    ret = pvs.pvSetGeometry(p,self.SpinBox1,550,10,111,30)

    ret = pvs.pvQDial(p,self.Dial1,self.tab1,2,101,13,3)
    ret = pvs.pvSetGeometry(p,self.Dial1,548,45,121,111)

    ret = pvs.pvQFrame(p,self.Line1,self.tab1,pvs.HLine,pvs.Plain,3,1)
    ret = pvs.pvSetGeometry(p,self.Line1,548,225,121,20)

    ret = pvs.pvQFrame(p,self.Line2,self.tab1,pvs.HLine,pvs.Plain,3,1)
    ret = pvs.pvSetGeometry(p,self.Line2,548,305,121,20)

    ret = pvs.pvQPushButton(p,self.PushButton1,self.tab1)
    ret = pvs.pvSetGeometry(p,self.PushButton1,8,15,101,31)
    ret = pvs.pvSetText(p,self.PushButton1,"Button1")
    ret = pvs.pvSetFont(p,self.PushButton1,"Bookman l",18,0,0,0,0)

    ret = pvs.pvQTable(p,self.Table1,self.tab1,11,11)
    ret = pvs.pvSetGeometry(p,self.Table1,8,165,531,261)

    ret = pvs.pvQGroupBox(p,self.GroupBox1,self.tab1,-1,pvs.HORIZONTAL,"GroupBox1")
    ret = pvs.pvSetGeometry(p,self.GroupBox1,238,5,151,150)
    ret = pvs.pvSetPaletteForegroundColor(p,self.GroupBox1,0,0,0)
    ret = pvs.pvSetPaletteBackgroundColor(p,self.GroupBox1,10,21,220)

    ret = pvs.pvQPushButton(p,self.PushButton4,self.GroupBox1)
    ret = pvs.pvSetGeometry(p,self.PushButton4,10,20,130,30)
    ret = pvs.pvSetText(p,self.PushButton4,"PushButton4")

    ret = pvs.pvQPushButton(p,self.PushButton5,self.GroupBox1)
    ret = pvs.pvSetGeometry(p,self.PushButton5,10,50,130,30)
    ret = pvs.pvSetText(p,self.PushButton5,"PushButton5")

    ret = pvs.pvQPushButton(p,self.PushButton6,self.GroupBox1)
    ret = pvs.pvSetGeometry(p,self.PushButton6,10,80,130,31)
    ret = pvs.pvSetText(p,self.PushButton6,"PushButton6")

    ret = pvs.pvQPushButton(p,self.PushButton7,self.GroupBox1)
    ret = pvs.pvSetGeometry(p,self.PushButton7,10,110,130,30)
    ret = pvs.pvSetText(p,self.PushButton7,"PushButton7")

    ret = pvs.pvQWidget(p,self.tab2,self.TabWidget2)
    ret = pvs.pvAddTab(p,self.TabWidget2,self.tab2,"Radio + Check + Edit")

    ret = pvs.pvQButtonGroup(p,self.ButtonGroup1,self.tab2,-1,pvs.HORIZONTAL,"ButtonGroup1")
    ret = pvs.pvSetGeometry(p,self.ButtonGroup1,20,20,151,140)

    ret = pvs.pvQRadioButton(p,self.RadioButton3,self.ButtonGroup1)
    ret = pvs.pvSetGeometry(p,self.RadioButton3,10,100,131,31)
    ret = pvs.pvSetText(p,self.RadioButton3,"RadioButton3")

    ret = pvs.pvQRadioButton(p,self.RadioButton2,self.ButtonGroup1)
    ret = pvs.pvSetGeometry(p,self.RadioButton2,10,60,131,31)
    ret = pvs.pvSetText(p,self.RadioButton2,"RadioButton2")

    ret = pvs.pvQRadioButton(p,self.RadioButton1,self.ButtonGroup1)
    ret = pvs.pvSetGeometry(p,self.RadioButton1,10,20,131,31)
    ret = pvs.pvSetText(p,self.RadioButton1,"RadioButton1")
    ret = pvs.pvSetChecked(p,self.RadioButton1,1)

    ret = pvs.pvQButtonGroup(p,self.ButtonGroup2,self.tab2,-1,pvs.HORIZONTAL,"ButtonGroup2")
    ret = pvs.pvSetGeometry(p,self.ButtonGroup2,20,170,150,140)

    ret = pvs.pvQCheckBox(p,self.CheckBox3,self.ButtonGroup2)
    ret = pvs.pvSetGeometry(p,self.CheckBox3,10,100,121,31)
    ret = pvs.pvSetText(p,self.CheckBox3,"CheckBox3")

    ret = pvs.pvQCheckBox(p,self.CheckBox2,self.ButtonGroup2)
    ret = pvs.pvSetGeometry(p,self.CheckBox2,10,60,121,31)
    ret = pvs.pvSetText(p,self.CheckBox2,"CheckBox2")

    ret = pvs.pvQCheckBox(p,self.CheckBox1,self.ButtonGroup2)
    ret = pvs.pvSetGeometry(p,self.CheckBox1,10,20,121,31)
    ret = pvs.pvSetText(p,self.CheckBox1,"CheckBox1")
    ret = pvs.pvSetChecked(p,self.CheckBox1,1)

    ret = pvs.pvQPushButton(p,self.PushButtonRequestMultiLineEdit,self.tab2)
    ret = pvs.pvSetGeometry(p,self.PushButtonRequestMultiLineEdit,18,315,151,31)
    ret = pvs.pvSetText(p,self.PushButtonRequestMultiLineEdit,"Request MultiLineEdit")

    ret = pvs.pvQMultiLineEdit(p,self.MultiLineEdit1,self.tab2,0,30)
    ret = pvs.pvSetGeometry(p,self.MultiLineEdit1,180,10,491,411)

    ret = pvs.pvQWidget(p,self.tab3,self.TabWidget2)
    ret = pvs.pvAddTab(p,self.TabWidget2,self.tab3,"Edit + Combo + Slider + Browser")

    ret = pvs.pvQLabel(p,self.TextLabel3,self.tab3)
    ret = pvs.pvSetGeometry(p,self.TextLabel3,168,15,101,31)
    ret = pvs.pvSetText(p,self.TextLabel3,"Edit")

    ret = pvs.pvQComboBox(p,self.ComboBox1,self.tab3,0,pvs.AtTop)
    ret = pvs.pvSetGeometry(p,self.ComboBox1,18,55,141,31)

    ret = pvs.pvQLabel(p,self.TextLabel4,self.tab3)
    ret = pvs.pvSetGeometry(p,self.TextLabel4,168,55,101,31)
    ret = pvs.pvSetText(p,self.TextLabel4,"Combo")

    ret = pvs.pvQLabel(p,self.TextLabel5,self.tab3)
    ret = pvs.pvSetGeometry(p,self.TextLabel5,168,95,101,31)
    ret = pvs.pvSetText(p,self.TextLabel5,"Combo editable")

    ret = pvs.pvQComboBox(p,self.ComboBox2,self.tab3,1,pvs.AtTop)
    ret = pvs.pvSetGeometry(p,self.ComboBox2,18,95,141,31)

    ret = pvs.pvQSlider(p,self.Slider1,self.tab3,0,99,10,0,pvs.Horizontal)
    ret = pvs.pvSetGeometry(p,self.Slider1,18,145,141,31)

    ret = pvs.pvQLabel(p,self.TextLabel6,self.tab3)
    ret = pvs.pvSetGeometry(p,self.TextLabel6,168,145,101,31)
    ret = pvs.pvSetText(p,self.TextLabel6,"Slider")

    ret = pvs.pvQSlider(p,self.Slider2,self.tab3,0,99,10,0,pvs.Vertical)
    ret = pvs.pvSetGeometry(p,self.Slider2,18,185,31,141)

    ret = pvs.pvQPushButton(p,self.PushButtonBackward,self.tab3)
    ret = pvs.pvSetGeometry(p,self.PushButtonBackward,330,10,50,20)
    ret = pvs.pvSetText(p,self.PushButtonBackward,"<-")

    ret = pvs.pvQPushButton(p,self.PushButtonForward,self.tab3)
    ret = pvs.pvSetGeometry(p,self.PushButtonForward,380,10,50,20)
    ret = pvs.pvSetText(p,self.PushButtonForward,"->")

    ret = pvs.pvQTextBrowser(p,self.TextBrowser1,self.tab3)
    ret = pvs.pvSetGeometry(p,self.TextBrowser1,278,36,381,380)

    ret = pvs.pvQPushButton(p,self.PushButtonHome,self.tab3)
    ret = pvs.pvSetGeometry(p,self.PushButtonHome,280,10,50,20)
    ret = pvs.pvSetText(p,self.PushButtonHome,"Home")

    ret = pvs.pvQLineEdit(p,self.LineEdit1,self.tab3)
    ret = pvs.pvSetGeometry(p,self.LineEdit1,18,15,141,31)
    ret = pvs.pvSetText(p,self.LineEdit1,"")
    ret = pvs.pvSetEchoMode(p,self.LineEdit1,2)

    ret = pvs.pvQWidget(p,self.tab4,self.TabWidget2)
    ret = pvs.pvAddTab(p,self.TabWidget2,self.tab4,"Image + Draw + Frames")

    ret = pvs.pvQDraw(p,self.Draw1,self.tab4)
    ret = pvs.pvSetGeometry(p,self.Draw1,298,5,371,280)

    ret = pvs.pvQLabel(p,self.TextLabel2_2,self.tab4)
    ret = pvs.pvSetGeometry(p,self.TextLabel2_2,20,300,300,31)
    ret = pvs.pvSetText(p,self.TextLabel2_2,"a JPG Image (you can also use it as a Webcam)")

    ret = pvs.pvQLabel(p,self.TextLabel1_2,self.tab4)
    ret = pvs.pvSetGeometry(p,self.TextLabel1_2,390,300,151,31)
    ret = pvs.pvSetText(p,self.TextLabel1_2,"a DrawImage")

    ret = pvs.pvQFrame(p,self.Frame3,self.tab4,pvs.StyledPanel,pvs.Raised,5,0)
    ret = pvs.pvSetGeometry(p,self.Frame3,8,5,278,278)

    ret = pvs.pvDownloadFile(p,"ajpg.jpg")
    ret = pvs.pvQImage(p,self.Image1,self.Frame3,"ajpg.jpg",w,h,depth)
    ret = pvs.pvSetGeometry(p,self.Image1,10,10,256,256)

    ret = pvs.pvQLabel(p,self.PixmapLabel1,self.tab4)
    ret = pvs.pvSetGeometry(p,self.PixmapLabel1,610,370,32,32)
    ret = pvs.pvSetText(p,self.PixmapLabel1,"")

    ret = pvs.pvQWidget(p,self.tab5,self.TabWidget2)
    ret = pvs.pvAddTab(p,self.TabWidget2,self.tab5,"ListView + IconView")

    ret = pvs.pvQIconView(p,self.IconView1,self.tab5)
    ret = pvs.pvSetGeometry(p,self.IconView1,338,15,331,401)

    ret = pvs.pvQPushButton(p,self.PushButtonRequestListView,self.tab5)
    ret = pvs.pvSetGeometry(p,self.PushButtonRequestListView,10,380,161,31)
    ret = pvs.pvSetText(p,self.PushButtonRequestListView,"Request Selection")

    ret = pvs.pvQListView(p,self.ListView1,self.tab5)
    ret = pvs.pvSetGeometry(p,self.ListView1,8,16,321,360)
    ret = pvs.pvSetMultiSelection(p,self.ListView1,1)

    ret = pvs.pvQWidget(p,self.tab6,self.TabWidget2)
    ret = pvs.pvAddTab(p,self.TabWidget2,self.tab6,"VTK")

    ret = pvs.pvQVtkTclWidget(p,self.VtkTclWidget1,self.tab6)
    ret = pvs.pvSetGeometry(p,self.VtkTclWidget1,8,4,531,421)

    ret = pvs.pvQSlider(p,self.VtkSlider1,self.tab6,0,99,10,50,pvs.Vertical)
    ret = pvs.pvSetGeometry(p,self.VtkSlider1,548,4,31,141)

    ret = pvs.pvQSlider(p,self.VtkSlider2,self.tab6,0,99,10,50,pvs.Horizontal)
    ret = pvs.pvSetGeometry(p,self.VtkSlider2,548,154,121,31)

    ret = pvs.pvEndDefinition(p)
    pvs.delete_int(w)
    pvs.delete_int(h)
    pvs.delete_int(depth)
    return 0

  #// _end_of_generated_area_ (do not edit -> use ui2pvc) ---------------------

  def defineMask(self,pvs,p):
    ret = self.generated_defineMask(pvs,p)
    # (todo: add your code here)
    return 0

  def showData(self,pvs,p):
    # (todo: add your code here)
    return 0

  def readData(self,pvs,p): # from shared memory, database or something else
    # (todo: add your code here)
    return 0

  def show(self,pvs,p):
    text = event = str1 = ""
    val = x = y = button = 0
    ret = self.defineMask(pvs,p)
    ret = self.readData(pvs,p) # from shared memory, database or something else
    ret = self.showData(pvs,p)
    while 1:
      event  = pvs.pvGetEvent(p)
      result = pvs.pvParseEventStruct(p,event)
      id     = result.event
      i      = result.i
      text   = result.text
      if id == pvs.NULL_EVENT:
        ret = self.readData(pvs,p) # from shared memory, database or something else
        ret = self.showData(pvs,p)
      elif id == pvs.BUTTON_EVENT:
        print 'BUTTON_EVENT id=', i
      elif id == pvs.BUTTON_PRESSED_EVENT:
        print 'BUTTON_PRESSED_EVENT id=' ,i
      elif id == pvs.BUTTON_RELEASED_EVENT:
        print 'BUTTON_RELEASED_EVENT id=' ,i
      elif id == pvs.TEXT_EVENT:
        print 'TEXT_EVENT id=',i,' text=',text
      elif id == pvs.SLIDER_EVENT:
        val = eval(text)
        print 'SLIDER_EVENT val=',val
      elif id == pvs.CHECKBOX_EVENT:
        print 'CHECKBOX_EVENT id=',i,' text=',text
      elif id == pvs.RADIOBUTTON_EVENT:
        print 'RADIOBUTTON_EVENT id=',i,' text=',text
      elif id == pvs.GL_INITIALIZE_EVENT:
        print 'you have to call initializeGL()'
      elif id == pvs.GL_PAINT_EVENT:
        print 'you have to call paintGL()'
      elif id == pvs.GL_RESIZE_EVENT:
        #sscanf(text,"(%d,%d)",&w,&h);
        tmp = text[1:-1]
        tmplist = string.split(tmp,',')
        w = int(tmplist[0])
        h = int(tmplist[1])
        print 'you have to call resizeGL(',w,',',h,')'
      elif id == pvs.GL_IDLE_EVENT:
        print 'glIdle()'
      elif id == pvs.TAB_EVENT:
        val = eval(text)
        print 'TAB_EVENT ',i,' val=',val
      elif id == pvs.TABLE_TEXT_EVENT:
        #sscanf(text,"(%d,%d,",&x,&y);
        tmp = text[1:-1]
        tmplist = string.split(tmp,',')
        x = int(tmplist[0])
        y = int(tmplist[1])
        str1 = pvs.pvGetTextParam(p,text)
        print 'TABLE_TEXT_EVENT x=' ,x , ' y=' ,y , ' str=', str1
      elif id == pvs.TABLE_CLICKED_EVENT:
        #sscanf(text,"(%d,%d,%d)",&x,&y,&button);
        tmp = text[1:-1]
        tmplist = string.split(tmp,',')
        x = int(tmplist[0])
        y = int(tmplist[1])
        button = int(tmplist[2])
        print 'TABLE_CLICKED_EVENT x=',x,' y=',y,' button=',button
      elif id == pvs.SELECTION_EVENT:
        val = eval(text)
        str1 = pvs.pvGetTextParam(p,text)
        print 'SELECTION_EVENT(column=',val,' str1=',str1
      elif id == pvs.CLIPBOARD_EVENT:
        val = eval(text)
        print 'CLIPBOARD_EVENT(id=' ,val
        print 'clipboard = ' ,p.clipboard
      elif id == pvs.RIGHT_MOUSE_EVENT:
        print 'RIGHT_MOUSE_EVENT id=' ,i
        #ret = pvs.pvPopupMenu(p,-1,"Menu1,Menu2,,Menu3")
      elif id == pvs.KEYBOARD_EVENT:
        val = eval(text)
        print 'KEYBOARD_EVENT modifier=',i,' val=',val
      else:
        print 'UNKNOWN_EVENT id=',i,' text=',text
