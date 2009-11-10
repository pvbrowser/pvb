#////////////////////////////////////////////////////////////////////////////
#//
#// show_periodic for ProcessViewServer created: Sun Jul 20 15:19:02 2003
#//
#////////////////////////////////////////////////////////////////////////////
package periodic;
use String::Scanf;

#// _begin_of_generated_area_ (do not edit -> use ui2pvc) -------------------

# our mask contains the following objects
my $ID_MAIN_WIDGET = 0;
my $PushButtonBack = 1;
my $TabWidget2 = 2;
my $tab1 = 3;
my $PushButton2 = 4;
my $TextLabel1 = 5;
my $TextLabel2 = 6;
my $ListBox1 = 7;
my $LCDNumber1 = 8;
my $ProgressBar1 = 9;
my $SpinBox1 = 10;
my $Dial1 = 11;
my $Line1 = 12;
my $Line2 = 13;
my $PushButton1 = 14;
my $Table1 = 15;
my $GroupBox1 = 16;
my $PushButton4 = 17;
my $PushButton5 = 18;
my $PushButton6 = 19;
my $PushButton7 = 20;
my $tab2 = 21;
my $ButtonGroup1 = 22;
my $RadioButton3 = 23;
my $RadioButton2 = 24;
my $RadioButton1 = 25;
my $ButtonGroup2 = 26;
my $CheckBox3 = 27;
my $CheckBox2 = 28;
my $CheckBox1 = 29;
my $PushButtonRequestMultiLineEdit = 30;
my $MultiLineEdit1 = 31;
my $tab3 = 32;
my $TextLabel3 = 33;
my $ComboBox1 = 34;
my $TextLabel4 = 35;
my $TextLabel5 = 36;
my $ComboBox2 = 37;
my $Slider1 = 38;
my $TextLabel6 = 39;
my $Slider2 = 40;
my $PushButtonBackward = 41;
my $PushButtonForward = 42;
my $TextBrowser1 = 43;
my $PushButtonHome = 44;
my $LineEdit1 = 45;
my $tab4 = 46;
my $Draw1 = 47;
my $TextLabel2_2 = 48;
my $TextLabel1_2 = 49;
my $Frame3 = 50;
my $Image1 = 51;
my $PixmapLabel1 = 52;
my $tab5 = 53;
my $IconView1 = 54;
my $PushButtonRequestListView = 55;
my $ListView1 = 56;
my $tab6 = 57;
my $VtkTclWidget1 = 58;
my $VtkSlider1 = 59;
my $VtkSlider2 = 60;
my $ID_END_OF_WIDGETS = 61;

sub new
{
  my $classname = shift;
  my $ptr = {};
  bless($ptr,$classname);
  return $ptr;
}

sub generated_defineMask
{
  my $pv  = shift;
  my $p   = shift;
  $w      = pv::new_int(0);
  $h      = pv::new_int(0);
  $depth  = pv::new_int(0);
  $ret = pv::pvStartDefinition($p,$ID_END_OF_WIDGETS);

  $ret = pv::pvQPushButton($p,$PushButtonBack,0);
  $ret = pv::pvSetGeometry($p,$PushButtonBack,10,480,111,40);
  $ret = pv::pvSetText($p,$PushButtonBack,"Back");

  $ret = pv::pvQTabWidget($p,$TabWidget2,0);
  $ret = pv::pvSetGeometry($p,$TabWidget2,10,10,680,460);

  $ret = pv::pvQWidget($p,$tab1,$TabWidget2);
  $ret = pv::pvAddTab($p,$TabWidget2,$tab1,"Labels + Buttons + More");

  $ret = pv::pvQPushButton($p,$PushButton2,$tab1);
  $ret = pv::pvSetGeometry($p,$PushButton2,8,55,101,31);
  $ret = pv::pvSetText($p,$PushButton2,"PushButton2");

  $ret = pv::pvQLabel($p,$TextLabel1,$tab1);
  $ret = pv::pvSetGeometry($p,$TextLabel1,128,15,101,31);
  $ret = pv::pvSetText($p,$TextLabel1,"TextLabel1");

  $ret = pv::pvQLabel($p,$TextLabel2,$tab1);
  $ret = pv::pvSetGeometry($p,$TextLabel2,128,55,101,31);
  $ret = pv::pvSetText($p,$TextLabel2,"TextLabel2");

  $ret = pv::pvQListBox($p,$ListBox1,$tab1);
  $ret = pv::pvSetGeometry($p,$ListBox1,400,10,140,140);

  $ret = pv::pvQLCDNumber($p,$LCDNumber1,$tab1,2,pv::Outline,pv::Dec);
  $ret = pv::pvSetGeometry($p,$LCDNumber1,550,170,111,41);

  $ret = pv::pvQProgressBar($p,$ProgressBar1,$tab1,50);
  $ret = pv::pvSetGeometry($p,$ProgressBar1,548,265,121,31);

  $ret = pv::pvQSpinBox($p,$SpinBox1,$tab1,3,112,1);
  $ret = pv::pvSetGeometry($p,$SpinBox1,550,10,111,30);

  $ret = pv::pvQDial($p,$Dial1,$tab1,2,101,13,3);
  $ret = pv::pvSetGeometry($p,$Dial1,548,45,121,111);

  $ret = pv::pvQFrame($p,$Line1,$tab1,pv::HLine,pv::Plain,3,1);
  $ret = pv::pvSetGeometry($p,$Line1,548,225,121,20);

  $ret = pv::pvQFrame($p,$Line2,$tab1,pv::HLine,pv::Plain,3,1);
  $ret = pv::pvSetGeometry($p,$Line2,548,305,121,20);

  $ret = pv::pvQPushButton($p,$PushButton1,$tab1);
  $ret = pv::pvSetGeometry($p,$PushButton1,8,15,101,31);
  $ret = pv::pvSetText($p,$PushButton1,"Button1");
  $ret = pv::pvSetFont($p,$PushButton1,"Bookman l",18,0,0,0,0);

  $ret = pv::pvQTable($p,$Table1,$tab1,11,11);
  $ret = pv::pvSetGeometry($p,$Table1,8,165,531,261);

  $ret = pv::pvQGroupBox($p,$GroupBox1,$tab1,-1,pv::HORIZONTAL,"GroupBox1");
  $ret = pv::pvSetGeometry($p,$GroupBox1,238,5,151,150);
  $ret = pv::pvSetPaletteForegroundColor($p,$GroupBox1,0,0,0);
  $ret = pv::pvSetPaletteBackgroundColor($p,$GroupBox1,10,21,220);

  $ret = pv::pvQPushButton($p,$PushButton4,$GroupBox1);
  $ret = pv::pvSetGeometry($p,$PushButton4,10,20,130,30);
  $ret = pv::pvSetText($p,$PushButton4,"PushButton4");

  $ret = pv::pvQPushButton($p,$PushButton5,$GroupBox1);
  $ret = pv::pvSetGeometry($p,$PushButton5,10,50,130,30);
  $ret = pv::pvSetText($p,$PushButton5,"PushButton5");

  $ret = pv::pvQPushButton($p,$PushButton6,$GroupBox1);
  $ret = pv::pvSetGeometry($p,$PushButton6,10,80,130,31);
  $ret = pv::pvSetText($p,$PushButton6,"PushButton6");

  $ret = pv::pvQPushButton($p,$PushButton7,$GroupBox1);
  $ret = pv::pvSetGeometry($p,$PushButton7,10,110,130,30);
  $ret = pv::pvSetText($p,$PushButton7,"PushButton7");

  $ret = pv::pvQWidget($p,$tab2,$TabWidget2);
  $ret = pv::pvAddTab($p,$TabWidget2,$tab2,"Radio + Check + Edit");

  $ret = pv::pvQButtonGroup($p,$ButtonGroup1,$tab2,-1,pv::HORIZONTAL,"ButtonGroup1");
  $ret = pv::pvSetGeometry($p,$ButtonGroup1,20,20,151,140);

  $ret = pv::pvQRadioButton($p,$RadioButton3,$ButtonGroup1);
  $ret = pv::pvSetGeometry($p,$RadioButton3,10,100,131,31);
  $ret = pv::pvSetText($p,$RadioButton3,"RadioButton3");

  $ret = pv::pvQRadioButton($p,$RadioButton2,$ButtonGroup1);
  $ret = pv::pvSetGeometry($p,$RadioButton2,10,60,131,31);
  $ret = pv::pvSetText($p,$RadioButton2,"RadioButton2");

  $ret = pv::pvQRadioButton($p,$RadioButton1,$ButtonGroup1);
  $ret = pv::pvSetGeometry($p,$RadioButton1,10,20,131,31);
  $ret = pv::pvSetText($p,$RadioButton1,"RadioButton1");
  $ret = pv::pvSetChecked($p,$RadioButton1,1);

  $ret = pv::pvQButtonGroup($p,$ButtonGroup2,$tab2,-1,pv::HORIZONTAL,"ButtonGroup2");
  $ret = pv::pvSetGeometry($p,$ButtonGroup2,20,170,150,140);

  $ret = pv::pvQCheckBox($p,$CheckBox3,$ButtonGroup2);
  $ret = pv::pvSetGeometry($p,$CheckBox3,10,100,121,31);
  $ret = pv::pvSetText($p,$CheckBox3,"CheckBox3");

  $ret = pv::pvQCheckBox($p,$CheckBox2,$ButtonGroup2);
  $ret = pv::pvSetGeometry($p,$CheckBox2,10,60,121,31);
  $ret = pv::pvSetText($p,$CheckBox2,"CheckBox2");

  $ret = pv::pvQCheckBox($p,$CheckBox1,$ButtonGroup2);
  $ret = pv::pvSetGeometry($p,$CheckBox1,10,20,121,31);
  $ret = pv::pvSetText($p,$CheckBox1,"CheckBox1");
  $ret = pv::pvSetChecked($p,$CheckBox1,1);

  $ret = pv::pvQPushButton($p,$PushButtonRequestMultiLineEdit,$tab2);
  $ret = pv::pvSetGeometry($p,$PushButtonRequestMultiLineEdit,18,315,151,31);
  $ret = pv::pvSetText($p,$PushButtonRequestMultiLineEdit,"Request MultiLineEdit");

  $ret = pv::pvQMultiLineEdit($p,$MultiLineEdit1,$tab2,0,30);
  $ret = pv::pvSetGeometry($p,$MultiLineEdit1,180,10,491,411);

  $ret = pv::pvQWidget($p,$tab3,$TabWidget2);
  $ret = pv::pvAddTab($p,$TabWidget2,$tab3,"Edit + Combo + Slider + Browser");

  $ret = pv::pvQLabel($p,$TextLabel3,$tab3);
  $ret = pv::pvSetGeometry($p,$TextLabel3,168,15,101,31);
  $ret = pv::pvSetText($p,$TextLabel3,"Edit");

  $ret = pv::pvQComboBox($p,$ComboBox1,$tab3,0,pv::AtTop);
  $ret = pv::pvSetGeometry($p,$ComboBox1,18,55,141,31);

  $ret = pv::pvQLabel($p,$TextLabel4,$tab3);
  $ret = pv::pvSetGeometry($p,$TextLabel4,168,55,101,31);
  $ret = pv::pvSetText($p,$TextLabel4,"Combo");

  $ret = pv::pvQLabel($p,$TextLabel5,$tab3);
  $ret = pv::pvSetGeometry($p,$TextLabel5,168,95,101,31);
  $ret = pv::pvSetText($p,$TextLabel5,"Combo editable");

  $ret = pv::pvQComboBox($p,$ComboBox2,$tab3,1,pv::AtTop);
  $ret = pv::pvSetGeometry($p,$ComboBox2,18,95,141,31);

  $ret = pv::pvQSlider($p,$Slider1,$tab3,0,99,10,0,pv::Horizontal);
  $ret = pv::pvSetGeometry($p,$Slider1,18,145,141,31);

  $ret = pv::pvQLabel($p,$TextLabel6,$tab3);
  $ret = pv::pvSetGeometry($p,$TextLabel6,168,145,101,31);
  $ret = pv::pvSetText($p,$TextLabel6,"Slider");

  $ret = pv::pvQSlider($p,$Slider2,$tab3,0,99,10,0,pv::Vertical);
  $ret = pv::pvSetGeometry($p,$Slider2,18,185,31,141);

  $ret = pv::pvQPushButton($p,$PushButtonBackward,$tab3);
  $ret = pv::pvSetGeometry($p,$PushButtonBackward,330,10,50,20);
  $ret = pv::pvSetText($p,$PushButtonBackward,"<-");

  $ret = pv::pvQPushButton($p,$PushButtonForward,$tab3);
  $ret = pv::pvSetGeometry($p,$PushButtonForward,380,10,50,20);
  $ret = pv::pvSetText($p,$PushButtonForward,"->");

  $ret = pv::pvQTextBrowser($p,$TextBrowser1,$tab3);
  $ret = pv::pvSetGeometry($p,$TextBrowser1,278,36,381,380);

  $ret = pv::pvQPushButton($p,$PushButtonHome,$tab3);
  $ret = pv::pvSetGeometry($p,$PushButtonHome,280,10,50,20);
  $ret = pv::pvSetText($p,$PushButtonHome,"Home");

  $ret = pv::pvQLineEdit($p,$LineEdit1,$tab3);
  $ret = pv::pvSetGeometry($p,$LineEdit1,18,15,141,31);
  $ret = pv::pvSetText($p,$LineEdit1,"");
  $ret = pv::pvSetEchoMode($p,$LineEdit1,2);

  $ret = pv::pvQWidget($p,$tab4,$TabWidget2);
  $ret = pv::pvAddTab($p,$TabWidget2,$tab4,"Image + Draw + Frames");

  $ret = pv::pvQDraw($p,$Draw1,$tab4);
  $ret = pv::pvSetGeometry($p,$Draw1,298,5,371,280);

  $ret = pv::pvQLabel($p,$TextLabel2_2,$tab4);
  $ret = pv::pvSetGeometry($p,$TextLabel2_2,20,300,300,31);
  $ret = pv::pvSetText($p,$TextLabel2_2,"a JPG Image (you can also use it as a Webcam)");

  $ret = pv::pvQLabel($p,$TextLabel1_2,$tab4);
  $ret = pv::pvSetGeometry($p,$TextLabel1_2,390,300,151,31);
  $ret = pv::pvSetText($p,$TextLabel1_2,"a DrawImage");

  $ret = pv::pvQFrame($p,$Frame3,$tab4,pv::StyledPanel,pv::Raised,5,0);
  $ret = pv::pvSetGeometry($p,$Frame3,8,5,278,278);

  $ret = pv::pvDownloadFile($p,"ajpg.jpg");
  $ret = pv::pvQImage($p,$Image1,$Frame3,"ajpg.jpg",$w,$h,$depth);
  $ret = pv::pvSetGeometry($p,$Image1,10,10,256,256);

  $ret = pv::pvQLabel($p,$PixmapLabel1,$tab4);
  $ret = pv::pvSetGeometry($p,$PixmapLabel1,610,370,32,32);
  $ret = pv::pvSetText($p,$PixmapLabel1,"");

  $ret = pv::pvQWidget($p,$tab5,$TabWidget2);
  $ret = pv::pvAddTab($p,$TabWidget2,$tab5,"ListView + IconView");

  $ret = pv::pvQIconView($p,$IconView1,$tab5);
  $ret = pv::pvSetGeometry($p,$IconView1,338,15,331,401);

  $ret = pv::pvQPushButton($p,$PushButtonRequestListView,$tab5);
  $ret = pv::pvSetGeometry($p,$PushButtonRequestListView,10,380,161,31);
  $ret = pv::pvSetText($p,$PushButtonRequestListView,"Request Selection");

  $ret = pv::pvQListView($p,$ListView1,$tab5);
  $ret = pv::pvSetGeometry($p,$ListView1,8,16,321,360);
  $ret = pv::pvSetMultiSelection($p,$ListView1,1);

  $ret = pv::pvQWidget($p,$tab6,$TabWidget2);
  $ret = pv::pvAddTab($p,$TabWidget2,$tab6,"VTK");

  $ret = pv::pvQVtkTclWidget($p,$VtkTclWidget1,$tab6);
  $ret = pv::pvSetGeometry($p,$VtkTclWidget1,8,4,531,421);

  $ret = pv::pvQSlider($p,$VtkSlider1,$tab6,0,99,10,50,pv::Vertical);
  $ret = pv::pvSetGeometry($p,$VtkSlider1,548,4,31,141);

  $ret = pv::pvQSlider($p,$VtkSlider2,$tab6,0,99,10,50,pv::Horizontal);
  $ret = pv::pvSetGeometry($p,$VtkSlider2,548,154,121,31);

  $ret = pv::pvEndDefinition($p);
  pv::delete_int($w);
  pv::delete_int($h);
  pv::delete_int($depth);
  return 0;
}

#// _end_of_generated_area_ (do not edit -> use ui2pvc) ---------------------

sub defineMask
{
  my $pv  = shift;
  my $p   = shift;
  $ret = generated_defineMask($pv,$p);
  # (todo: add your code here)
  return 0;
}

sub showData
{
  my $pv  = shift;
  my $p   = shift;
  # (todo: add your code here)
  return 0;
}

sub readData # from shared memory, database or something else
{
  my $pv  = shift;
  my $p   = shift;
  # (todo: add your code here)
  return 0;
}

sub show
{
  my $obj = shift;
  my $pv  = shift;
  my $p   = shift;
  $text = $event = $str1 = "";
  $val = $x = $y = $button = 0;
  $ret = defineMask($pv,$p);
  $ret = readData($pv,$p); # from shared memory, database or something else
  $ret = showData($pv,$p);
  while(1)
  {
    $event  = pv::pvGetEvent($p);
    $result = pv::pvParseEventStruct($p,$event);
    $id     = pv::PARSE_EVENT_STRUCT_event_get($result);
    $i      = pv::PARSE_EVENT_STRUCT_i_get($result);
    $text   = pv::PARSE_EVENT_STRUCT_text_get($result);
    if($id == $pv::NULL_EVENT)
    {
      $ret = readData($pv,$p); # from shared memory, database or something else
      $ret = showData($pv,$p);
    }
    elsif($id == $pv::BUTTON_EVENT)
    {
      print "BUTTON_EVENT id=", $i, "\n";
    }
    elsif($id == $pv::BUTTON_PRESSED_EVENT)
    {
      print "BUTTON_PRESSED_EVENT id=", $i, "\n";
    }
    elsif($id == $pv::BUTTON_RELEASED_EVENT)
    {
      print "BUTTON_RELEASED_EVENT id=", $i, "\n";
    }
    elsif($id == $pv::TEXT_EVENT)
    {
      print "TEXT_EVENT id=", $i," text=", $text, "\n";
    }
    elsif($id == $pv::SLIDER_EVENT)
    {
      ($val) = sscanf("(%d)",$text);
      print "SLIDER_EVENT val=", $val, "\n";
    }
    elsif($id == $pv::CHECKBOX_EVENT)
    {
      print "CHECKBOX_EVENT id=", $i, " text=", $text, "\n";
    }
    elsif($id == $pv::RADIOBUTTON_EVENT)
    {
      print "RADIOBUTTON_EVENT id=", $i, " text=", $text, "\n";
    }
    elsif($id == $pv::GL_INITIALIZE_EVENT)
    {
      print "you have to call initializeGL()\n";
    }
    elsif($id == $pv::GL_PAINT_EVENT)
    {
      print "you have to call paintGL()\n";
    }
    elsif($id == $pv::GL_RESIZE_EVENT)
    {
      ($w, $h) = sscanf("(%d,%d)",$text);
      print "you have to call resizeGL(",$w,",",$h,")\n;"
    }
    elsif($id == $pv::GL_IDLE_EVENT)
    {
      print "glIdle()\n";
    }
    elsif($id == $pv::TAB_EVENT)
    {
      ($val) = sscanf("(%d)",$text);
      print "TAB_EVENT ",$i," val=",$val,"\n";
    }
    elsif($id == $pv::TABLE_TEXT_EVENT)
    {
      ($x, $y) = sscanf("(%d,%d)",$text);
      $str1 = pv::pvGetTextParam($p,$text);
      print "TABLE_TEXT_EVENT x=" ,$x , " y=" ,$y , " str=", $str1, "\n";
    }
    elsif($id == $pv::TABLE_CLICKED_EVENT)
    {
      ($x, $y, $button) = sscanf("(%d,%d,%d)",$text);
      print "TABLE_CLICKED_EVENT x=",$x," y=",$y," button=",$button,"\n";
    }
    elsif($id == $pv::SELECTION_EVENT)
    {
      ($val) = sscanf("(%d)",$text);
      $str1 = pv::pvGetTextParam(p,text);
      print "SELECTION_EVENT(column=",$val," str1=",$str1,"\n";
    }
    elsif($id == $pv::CLIPBOARD_EVENT)
    {
      ($val) = sscanf("(%d)",$text);
      print "CLIPBOARD_EVENT(id=" ,$val, "\n";
      print "clipboard = " ,$p.clipboard,"\n";
    }
    elsif($id == $pv::RIGHT_MOUSE_EVENT)
    {
      print "RIGHT_MOUSE_EVENT id=" ,$i,"\n";
      #$ret = pv.pvPopupMenu($p,-1,"Menu1,Menu2,,Menu3\n");
    }
    elsif($id == $pv::KEYBOARD_EVENT)
    {
      ($val) = sscanf("(%d)",$text);
      print "KEYBOARD_EVENT modifier=",$i," val=",$val,"\n";
    }
    else
    {
      print "UNKNOWN_EVENT id=",$id, " i=", $i," text=",$text, "\n";
    }
  }
}

1;
