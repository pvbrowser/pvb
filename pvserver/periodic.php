<?
#///////////////////////////////////////////////////////////////////////////
#//
#// show_periodic for ProcessViewServer created: Tue Jul 22 10:49:00 2003
#//
#///////////////////////////////////////////////////////////////////////////

class periodic
{
  #// _begin_of_generated_area_ (do not edit -> use ui2pvc) ----------------

  # our mask contains the following objects
  var $ID_MAIN_WIDGET = 0;
  var $PushButtonBack = 1;
  var $TabWidget2 = 2;
  var $tab1 = 3;
  var $PushButton2 = 4;
  var $TextLabel1 = 5;
  var $TextLabel2 = 6;
  var $ListBox1 = 7;
  var $LCDNumber1 = 8;
  var $ProgressBar1 = 9;
  var $SpinBox1 = 10;
  var $Dial1 = 11;
  var $Line1 = 12;
  var $Line2 = 13;
  var $PushButton1 = 14;
  var $Table1 = 15;
  var $GroupBox1 = 16;
  var $PushButton4 = 17;
  var $PushButton5 = 18;
  var $PushButton6 = 19;
  var $PushButton7 = 20;
  var $tab2 = 21;
  var $ButtonGroup1 = 22;
  var $RadioButton3 = 23;
  var $RadioButton2 = 24;
  var $RadioButton1 = 25;
  var $ButtonGroup2 = 26;
  var $CheckBox3 = 27;
  var $CheckBox2 = 28;
  var $CheckBox1 = 29;
  var $PushButtonRequestMultiLineEdit = 30;
  var $MultiLineEdit1 = 31;
  var $tab3 = 32;
  var $TextLabel3 = 33;
  var $ComboBox1 = 34;
  var $TextLabel4 = 35;
  var $TextLabel5 = 36;
  var $ComboBox2 = 37;
  var $Slider1 = 38;
  var $TextLabel6 = 39;
  var $Slider2 = 40;
  var $PushButtonBackward = 41;
  var $PushButtonForward = 42;
  var $TextBrowser1 = 43;
  var $PushButtonHome = 44;
  var $LineEdit1 = 45;
  var $tab4 = 46;
  var $Draw1 = 47;
  var $TextLabel2_2 = 48;
  var $TextLabel1_2 = 49;
  var $Frame3 = 50;
  var $Image1 = 51;
  var $PixmapLabel1 = 52;
  var $tab5 = 53;
  var $IconView1 = 54;
  var $PushButtonRequestListView = 55;
  var $ListView1 = 56;
  var $tab6 = 57;
  var $VtkTclWidget1 = 58;
  var $VtkSlider1 = 59;
  var $VtkSlider2 = 60;
  var $ID_END_OF_WIDGETS = 61;

  function generated_defineMask($p)
  {
    $w     = new_int(0);
    $h     = new_int(0);
    $depth = new_int(0);
    $ret = pvStartDefinition($p,$this->ID_END_OF_WIDGETS);


    $ret = pvQPushButton($p,$this->PushButtonBack,0);
    $ret = pvSetGeometry($p,$this->PushButtonBack,10,480,111,40);
    $ret = pvSetText($p,$this->PushButtonBack,"Back");

    $ret = pvQTabWidget($p,$this->TabWidget2,0);
    $ret = pvSetGeometry($p,$this->TabWidget2,10,10,680,460);

    $ret = pvQWidget($p,$this->tab1,$this->TabWidget2);
    $ret = pvAddTab($p,$this->TabWidget2,$this->tab1,"Labels + Buttons + More");

    $ret = pvQPushButton($p,$this->PushButton2,$this->tab1);
    $ret = pvSetGeometry($p,$this->PushButton2,8,55,101,31);
    $ret = pvSetText($p,$this->PushButton2,"PushButton2");

    $ret = pvQLabel($p,$this->TextLabel1,$this->tab1);
    $ret = pvSetGeometry($p,$this->TextLabel1,128,15,101,31);
    $ret = pvSetText($p,$this->TextLabel1,"TextLabel1");

    $ret = pvQLabel($p,$this->TextLabel2,$this->tab1);
    $ret = pvSetGeometry($p,$this->TextLabel2,128,55,101,31);
    $ret = pvSetText($p,$this->TextLabel2,"TextLabel2");

    $ret = pvQListBox($p,$this->ListBox1,$this->tab1);
    $ret = pvSetGeometry($p,$this->ListBox1,400,10,140,140);

    $ret = pvQLCDNumber($p,$this->LCDNumber1,$this->tab1,2,Outline,Dec);
    $ret = pvSetGeometry($p,$this->LCDNumber1,550,170,111,41);

    $ret = pvQProgressBar($p,$this->ProgressBar1,$this->tab1,50);
    $ret = pvSetGeometry($p,$this->ProgressBar1,548,265,121,31);

    $ret = pvQSpinBox($p,$this->SpinBox1,$this->tab1,3,112,1);
    $ret = pvSetGeometry($p,$this->SpinBox1,550,10,111,30);

    $ret = pvQDial($p,$this->Dial1,$this->tab1,2,101,13,3);
    $ret = pvSetGeometry($p,$this->Dial1,548,45,121,111);

    $ret = pvQFrame($p,$this->Line1,$this->tab1,HLine,Plain,3,1);
    $ret = pvSetGeometry($p,$this->Line1,548,225,121,20);

    $ret = pvQFrame($p,$this->Line2,$this->tab1,HLine,Plain,3,1);
    $ret = pvSetGeometry($p,$this->Line2,548,305,121,20);

    $ret = pvQPushButton($p,$this->PushButton1,$this->tab1);
    $ret = pvSetGeometry($p,$this->PushButton1,8,15,101,31);
    $ret = pvSetText($p,$this->PushButton1,"Button1");
    $ret = pvSetFont($p,$this->PushButton1,"Bookman l",18,0,0,0,0);

    $ret = pvQTable($p,$this->Table1,$this->tab1,11,11);
    $ret = pvSetGeometry($p,$this->Table1,8,165,531,261);

    $ret = pvQGroupBox($p,$this->GroupBox1,$this->tab1,-1,HORIZONTAL,"GroupBox1");
    $ret = pvSetGeometry($p,$this->GroupBox1,238,5,151,150);
    $ret = pvSetPaletteForegroundColor($p,$this->GroupBox1,0,0,0);
    $ret = pvSetPaletteBackgroundColor($p,$this->GroupBox1,10,21,220);

    $ret = pvQPushButton($p,$this->PushButton4,$this->GroupBox1);
    $ret = pvSetGeometry($p,$this->PushButton4,10,20,130,30);
    $ret = pvSetText($p,$this->PushButton4,"PushButton4");

    $ret = pvQPushButton($p,$this->PushButton5,$this->GroupBox1);
    $ret = pvSetGeometry($p,$this->PushButton5,10,50,130,30);
    $ret = pvSetText($p,$this->PushButton5,"PushButton5");

    $ret = pvQPushButton($p,$this->PushButton6,$this->GroupBox1);
    $ret = pvSetGeometry($p,$this->PushButton6,10,80,130,31);
    $ret = pvSetText($p,$this->PushButton6,"PushButton6");

    $ret = pvQPushButton($p,$this->PushButton7,$this->GroupBox1);
    $ret = pvSetGeometry($p,$this->PushButton7,10,110,130,30);
    $ret = pvSetText($p,$this->PushButton7,"PushButton7");

    $ret = pvQWidget($p,$this->tab2,$this->TabWidget2);
    $ret = pvAddTab($p,$this->TabWidget2,$this->tab2,"Radio + Check + Edit");

    $ret = pvQButtonGroup($p,$this->ButtonGroup1,$this->tab2,-1,HORIZONTAL,"ButtonGroup1");
    $ret = pvSetGeometry($p,$this->ButtonGroup1,20,20,151,140);

    $ret = pvQRadioButton($p,$this->RadioButton3,$this->ButtonGroup1);
    $ret = pvSetGeometry($p,$this->RadioButton3,10,100,131,31);
    $ret = pvSetText($p,$this->RadioButton3,"RadioButton3");

    $ret = pvQRadioButton($p,$this->RadioButton2,$this->ButtonGroup1);
    $ret = pvSetGeometry($p,$this->RadioButton2,10,60,131,31);
    $ret = pvSetText($p,$this->RadioButton2,"RadioButton2");

    $ret = pvQRadioButton($p,$this->RadioButton1,$this->ButtonGroup1);
    $ret = pvSetGeometry($p,$this->RadioButton1,10,20,131,31);
    $ret = pvSetText($p,$this->RadioButton1,"RadioButton1");
    $ret = pvSetChecked($p,$this->RadioButton1,1);

    $ret = pvQButtonGroup($p,$this->ButtonGroup2,$this->tab2,-1,HORIZONTAL,"ButtonGroup2");
    $ret = pvSetGeometry($p,$this->ButtonGroup2,20,170,150,140);

    $ret = pvQCheckBox($p,$this->CheckBox3,$this->ButtonGroup2);
    $ret = pvSetGeometry($p,$this->CheckBox3,10,100,121,31);
    $ret = pvSetText($p,$this->CheckBox3,"CheckBox3");

    $ret = pvQCheckBox($p,$this->CheckBox2,$this->ButtonGroup2);
    $ret = pvSetGeometry($p,$this->CheckBox2,10,60,121,31);
    $ret = pvSetText($p,$this->CheckBox2,"CheckBox2");

    $ret = pvQCheckBox($p,$this->CheckBox1,$this->ButtonGroup2);
    $ret = pvSetGeometry($p,$this->CheckBox1,10,20,121,31);
    $ret = pvSetText($p,$this->CheckBox1,"CheckBox1");
    $ret = pvSetChecked($p,$this->CheckBox1,1);

    $ret = pvQPushButton($p,$this->PushButtonRequestMultiLineEdit,$this->tab2);
    $ret = pvSetGeometry($p,$this->PushButtonRequestMultiLineEdit,18,315,151,31);
    $ret = pvSetText($p,$this->PushButtonRequestMultiLineEdit,"Request MultiLineEdit");

    $ret = pvQMultiLineEdit($p,$this->MultiLineEdit1,$this->tab2,0,30);
    $ret = pvSetGeometry($p,$this->MultiLineEdit1,180,10,491,411);

    $ret = pvQWidget($p,$this->tab3,$this->TabWidget2);
    $ret = pvAddTab($p,$this->TabWidget2,$this->tab3,"Edit + Combo + Slider + Browser");

    $ret = pvQLabel($p,$this->TextLabel3,$this->tab3);
    $ret = pvSetGeometry($p,$this->TextLabel3,168,15,101,31);
    $ret = pvSetText($p,$this->TextLabel3,"Edit");

    $ret = pvQComboBox($p,$this->ComboBox1,$this->tab3,0,AtTop);
    $ret = pvSetGeometry($p,$this->ComboBox1,18,55,141,31);

    $ret = pvQLabel($p,$this->TextLabel4,$this->tab3);
    $ret = pvSetGeometry($p,$this->TextLabel4,168,55,101,31);
    $ret = pvSetText($p,$this->TextLabel4,"Combo");

    $ret = pvQLabel($p,$this->TextLabel5,$this->tab3);
    $ret = pvSetGeometry($p,$this->TextLabel5,168,95,101,31);
    $ret = pvSetText($p,$this->TextLabel5,"Combo editable");

    $ret = pvQComboBox($p,$this->ComboBox2,$this->tab3,1,AtTop);
    $ret = pvSetGeometry($p,$this->ComboBox2,18,95,141,31);

    $ret = pvQSlider($p,$this->Slider1,$this->tab3,0,99,10,0,Horizontal);
    $ret = pvSetGeometry($p,$this->Slider1,18,145,141,31);

    $ret = pvQLabel($p,$this->TextLabel6,$this->tab3);
    $ret = pvSetGeometry($p,$this->TextLabel6,168,145,101,31);
    $ret = pvSetText($p,$this->TextLabel6,"Slider");

    $ret = pvQSlider($p,$this->Slider2,$this->tab3,0,99,10,0,Vertical);
    $ret = pvSetGeometry($p,$this->Slider2,18,185,31,141);

    $ret = pvQPushButton($p,$this->PushButtonBackward,$this->tab3);
    $ret = pvSetGeometry($p,$this->PushButtonBackward,330,10,50,20);
    $ret = pvSetText($p,$this->PushButtonBackward,"<-");

    $ret = pvQPushButton($p,$this->PushButtonForward,$this->tab3);
    $ret = pvSetGeometry($p,$this->PushButtonForward,380,10,50,20);
    $ret = pvSetText($p,$this->PushButtonForward,"->");

    $ret = pvQTextBrowser($p,$this->TextBrowser1,$this->tab3);
    $ret = pvSetGeometry($p,$this->TextBrowser1,278,36,381,380);

    $ret = pvQPushButton($p,$this->PushButtonHome,$this->tab3);
    $ret = pvSetGeometry($p,$this->PushButtonHome,280,10,50,20);
    $ret = pvSetText($p,$this->PushButtonHome,"Home");

    $ret = pvQLineEdit($p,$this->LineEdit1,$this->tab3);
    $ret = pvSetGeometry($p,$this->LineEdit1,18,15,141,31);
    $ret = pvSetText($p,$this->LineEdit1,"");
    $ret = pvSetEchoMode($p,$this->LineEdit1,2);

    $ret = pvQWidget($p,$this->tab4,$this->TabWidget2);
    $ret = pvAddTab($p,$this->TabWidget2,$this->tab4,"Image + Draw + Frames");

    $ret = pvQDraw($p,$this->Draw1,$this->tab4);
    $ret = pvSetGeometry($p,$this->Draw1,298,5,371,280);

    $ret = pvQLabel($p,$this->TextLabel2_2,$this->tab4);
    $ret = pvSetGeometry($p,$this->TextLabel2_2,20,300,300,31);
    $ret = pvSetText($p,$this->TextLabel2_2,"a JPG Image (you can also use it as a Webcam)");

    $ret = pvQLabel($p,$this->TextLabel1_2,$this->tab4);
    $ret = pvSetGeometry($p,$this->TextLabel1_2,390,300,151,31);
    $ret = pvSetText($p,$this->TextLabel1_2,"a DrawImage");

    $ret = pvQFrame($p,$this->Frame3,$this->tab4,StyledPanel,Raised,5,0);
    $ret = pvSetGeometry($p,$this->Frame3,8,5,278,278);

    $ret = pvDownloadFile($p,"ajpg.jpg");
    $ret = pvQImage($p,$this->Image1,$this->Frame3,"ajpg.jpg",$w,$h,$depth);
    $ret = pvSetGeometry($p,$this->Image1,10,10,256,256);

    $ret = pvQLabel($p,$this->PixmapLabel1,$this->tab4);
    $ret = pvSetGeometry($p,$this->PixmapLabel1,610,370,32,32);
    $ret = pvSetText($p,$this->PixmapLabel1,"");

    $ret = pvQWidget($p,$this->tab5,$this->TabWidget2);
    $ret = pvAddTab($p,$this->TabWidget2,$this->tab5,"ListView + IconView");

    $ret = pvQIconView($p,$this->IconView1,$this->tab5);
    $ret = pvSetGeometry($p,$this->IconView1,338,15,331,401);

    $ret = pvQPushButton($p,$this->PushButtonRequestListView,$this->tab5);
    $ret = pvSetGeometry($p,$this->PushButtonRequestListView,10,380,161,31);
    $ret = pvSetText($p,$this->PushButtonRequestListView,"Request Selection");

    $ret = pvQListView($p,$this->ListView1,$this->tab5);
    $ret = pvSetGeometry($p,$this->ListView1,8,16,321,360);
    $ret = pvSetMultiSelection($p,$this->ListView1,1);

    $ret = pvQWidget($p,$this->tab6,$this->TabWidget2);
    $ret = pvAddTab($p,$this->TabWidget2,$this->tab6,"VTK");

    $ret = pvQVtkTclWidget($p,$this->VtkTclWidget1,$this->tab6);
    $ret = pvSetGeometry($p,$this->VtkTclWidget1,8,4,531,421);

    $ret = pvQSlider($p,$this->VtkSlider1,$this->tab6,0,99,10,50,Vertical);
    $ret = pvSetGeometry($p,$this->VtkSlider1,548,4,31,141);

    $ret = pvQSlider($p,$this->VtkSlider2,$this->tab6,0,99,10,50,Horizontal);
    $ret = pvSetGeometry($p,$this->VtkSlider2,548,154,121,31);

    $ret = pvEndDefinition($p);
    delete_int($w);
    delete_int($h);
    delete_int($depth);
    return 0;
  }

  #// _end_of_generated_area_ (do not edit -> use ui2pvc) -------------

  function defineMask($p)
  {
    $ret = $this->generated_defineMask($p);
    # (todo: add your code here)
    return 0;
  }

  function showData($p)
  {
    # (todo: add your code here)
    return 0;
  }

  function readData($p) # from shared memory, database or something else
  {
    # (todo: add your code here)
    return 0;
  }

  function show($p)
  {
    $text = $event = $str1 = "";
    $val = $x = $y = $button = 0;
    $ret = $this->defineMask($p);
    $ret = $this->readData($p); # from shared memory, database or something else
    $ret = $this->showData($p);
    while(1)
    {
      $event  = pvGetEvent($p);
      $result = pvParseEventStruct($p,$event);
      $id     = $result->event;
      $i      = $result->i;
      $text   = $result->text;
      if($id == NULL_EVENT)
      {
        $ret = $this->readData($p); # from shared memory, database or something else
        $ret = $this->showData($p);
      }
      elseif($id == BUTTON_EVENT)
      {
        echo "BUTTON_EVENT id=$i\n";
      }
      elseif($id == BUTTON_PRESSED_EVENT)
      {
        echo "BUTTON_PRESSED_EVENT id=$i\n";
      }
      elseif($id == BUTTON_RELEASED_EVENT)
      {
        echo "BUTTON_RELEASED_EVENT id=$i\n";
      }
      elseif($id == TEXT_EVENT)
      {
        echo "TEXT_EVENT id=$i text=$text,\n";
      }
      elseif($id == SLIDER_EVENT)
      {
        list($val) = sscanf($text,"(%d)");
        echo "SLIDER_EVENT val=$val\n";
      }
      elseif($id == CHECKBOX_EVENT)
      {
        echo "CHECKBOX_EVENT id=$i text=$text\n";
      }
      elseif($id == RADIOBUTTON_EVENT)
      {
        echo "RADIOBUTTON_EVENT id=$i text=$text\n";
      }
      elseif($id == GL_INITIALIZE_EVENT)
      {
        echo "you have to call initializeGL()\n";
      }
      elseif($id == GL_PAINT_EVENT)
      {
        echo "you have to call paintGL()\n";
      }
      elseif($id == GL_RESIZE_EVENT)
      {
        list($w, $h) = sscanf($text,"(%d,%d)");
        echo "you have to call resizeGL($w,$h)\n";
      }
      elseif($id == GL_IDLE_EVENT)
      {
        echo "glIdle()\n";
      }
      elseif($id == TAB_EVENT)
      {
        list($val) = sscanf($text,"(%d)");
        echo "TAB_EVENT $i val=$val\n";
      }
      elseif($id == TABLE_TEXT_EVENT)
      {
        list($x, $y) = sscanf($text,"(%d,%d)");
        $str1 = pvGetTextParam($p,$text);
        echo "TABLE_TEXT_EVENT x=$x y=$y str=$str1\n";
      }
      elseif($id == TABLE_CLICKED_EVENT)
      {
        list($x, $y, $button) = sscanf($text,"(%d,%d,%d)");
        echo "TABLE_CLICKED_EVENT x=$x y=$y button=$button\n";
      }
      elseif($id == SELECTION_EVENT)
      {
        list($val) = sscanf($text,"(%d)");
        $str1 = pvGetTextParam($p,$text);
        echo "SELECTION_EVENT(column=$val str1=$str1\n";
      }
      elseif($id == CLIPBOARD_EVENT)
      {
        list($val) = sscanf($text,"(%d)");
        echo "CLIPBOARD_EVENT(id=$val)\n";
        echo "clipboard=$p->clipboard\n";
      }
      elseif($id == RIGHT_MOUSE_EVENT)
      {
        echo "RIGHT_MOUSE_EVENT id=$i\n";
        #$ret = pvPopupMenu($p,-1,"Menu1,Menu2,,Menu3\n");
      }
      elseif($id == KEYBOARD_EVENT)
      {
        list($val) = sscanf($text,"(%d)");
        echo "KEYBOARD_EVENT modifier=$i val=$val\n";
      }
      else
      {
        echo "UNKNOWN_EVENT id=$id i=$i text=$text\n";
      }
    }
  }
}
?>
