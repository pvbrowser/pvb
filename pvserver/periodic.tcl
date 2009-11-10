#////////////////////////////////////////////////////////////////////////////
#//
#// show_periodic for ProcessViewServer created: Wed Jul 30 11:40:49 2003
#//
#////////////////////////////////////////////////////////////////////////////
namespace eval periodic {
  namespace eval this {
    # _begin_of_generated_area_ (do not edit -> use ui2pvc) -----------------

    # our mask contains the following objects
    variable ID_MAIN_WIDGET 0
    variable PushButtonBack 1
    variable TabWidget2 2
    variable tab1 3
    variable PushButton2 4
    variable TextLabel1 5
    variable TextLabel2 6
    variable ListBox1 7
    variable LCDNumber1 8
    variable ProgressBar1 9
    variable SpinBox1 10
    variable Dial1 11
    variable Line1 12
    variable Line2 13
    variable PushButton1 14
    variable Table1 15
    variable GroupBox1 16
    variable PushButton4 17
    variable PushButton5 18
    variable PushButton6 19
    variable PushButton7 20
    variable tab2 21
    variable ButtonGroup1 22
    variable RadioButton3 23
    variable RadioButton2 24
    variable RadioButton1 25
    variable ButtonGroup2 26
    variable CheckBox3 27
    variable CheckBox2 28
    variable CheckBox1 29
    variable PushButtonRequestMultiLineEdit 30
    variable MultiLineEdit1 31
    variable tab3 32
    variable TextLabel3 33
    variable ComboBox1 34
    variable TextLabel4 35
    variable TextLabel5 36
    variable ComboBox2 37
    variable Slider1 38
    variable TextLabel6 39
    variable Slider2 40
    variable PushButtonBackward 41
    variable PushButtonForward 42
    variable TextBrowser1 43
    variable PushButtonHome 44
    variable LineEdit1 45
    variable tab4 46
    variable Draw1 47
    variable TextLabel2_2 48
    variable TextLabel1_2 49
    variable Frame3 50
    variable Image1 51
    variable PixmapLabel1 52
    variable tab5 53
    variable IconView1 54
    variable PushButtonRequestListView 55
    variable ListView1 56
    variable tab6 57
    variable VtkTclWidget1 58
    variable VtkSlider1 59
    variable VtkSlider2 60
    variable ID_END_OF_WIDGETS 61
  }

  proc generated_defineMask { p } {
    set w     [ new_int 0 ]
    set h     [ new_int 0 ]
    set depth [ new_int 0 ]
    pvStartDefinition $p $this::ID_END_OF_WIDGETS

    pvQPushButton $p $this::PushButtonBack 0
    pvSetGeometry $p $this::PushButtonBack 10 480 111 40
    pvSetText $p $this::PushButtonBack "Back"

    pvQTabWidget $p $this::TabWidget2 0
    pvSetGeometry $p $this::TabWidget2 10 10 680 460

    pvQWidget $p $this::tab1 $this::TabWidget2
    pvAddTab $p $this::TabWidget2 $this::tab1 "Labels + Buttons + More"

    pvQPushButton $p $this::PushButton2 $this::tab1
    pvSetGeometry $p $this::PushButton2 8 55 101 31
    pvSetText $p $this::PushButton2 "PushButton2"

    pvQLabel $p $this::TextLabel1 $this::tab1
    pvSetGeometry $p $this::TextLabel1 128 15 101 31
    pvSetText $p $this::TextLabel1 "TextLabel1"

    pvQLabel $p $this::TextLabel2 $this::tab1
    pvSetGeometry $p $this::TextLabel2 128 55 101 31
    pvSetText $p $this::TextLabel2 "TextLabel2"

    pvQListBox $p $this::ListBox1 $this::tab1
    pvSetGeometry $p $this::ListBox1 400 10 140 140

    pvQLCDNumber $p $this::LCDNumber1 $this::tab1 2 $::Outline $::Dec
    pvSetGeometry $p $this::LCDNumber1 550 170 111 41

    pvQProgressBar $p $this::ProgressBar1 $this::tab1 50
    pvSetGeometry $p $this::ProgressBar1 548 265 121 31

    pvQSpinBox $p $this::SpinBox1 $this::tab1 3 112 1
    pvSetGeometry $p $this::SpinBox1 550 10 111 30

    pvQDial $p $this::Dial1 $this::tab1 2 101 13 3
    pvSetGeometry $p $this::Dial1 548 45 121 111

    pvQFrame $p $this::Line1 $this::tab1 $::HLine $::Plain 3 1
    pvSetGeometry $p $this::Line1 548 225 121 20

    pvQFrame $p $this::Line2 $this::tab1 $::HLine $::Plain 3 1
    pvSetGeometry $p $this::Line2 548 305 121 20

    pvQPushButton $p $this::PushButton1 $this::tab1
    pvSetGeometry $p $this::PushButton1 8 15 101 31
    pvSetText $p $this::PushButton1 "Button1"
    pvSetFont $p $this::PushButton1 "Bookman l" 18 0 0 0 0

    pvQTable $p $this::Table1 $this::tab1 11 11
    pvSetGeometry $p $this::Table1 8 165 531 261

    pvQGroupBox $p $this::GroupBox1 $this::tab1 -1 $::HORIZONTAL "GroupBox1"
    pvSetGeometry $p $this::GroupBox1 238 5 151 150
    pvSetPaletteForegroundColor $p $this::GroupBox1 0 0 0
    pvSetPaletteBackgroundColor $p $this::GroupBox1 10 21 220

    pvQPushButton $p $this::PushButton4 $this::GroupBox1
    pvSetGeometry $p $this::PushButton4 10 20 130 30
    pvSetText $p $this::PushButton4 "PushButton4"

    pvQPushButton $p $this::PushButton5 $this::GroupBox1
    pvSetGeometry $p $this::PushButton5 10 50 130 30
    pvSetText $p $this::PushButton5 "PushButton5"

    pvQPushButton $p $this::PushButton6 $this::GroupBox1
    pvSetGeometry $p $this::PushButton6 10 80 130 31
    pvSetText $p $this::PushButton6 "PushButton6"

    pvQPushButton $p $this::PushButton7 $this::GroupBox1
    pvSetGeometry $p $this::PushButton7 10 110 130 30
    pvSetText $p $this::PushButton7 "PushButton7"

    pvQWidget $p $this::tab2 $this::TabWidget2
    pvAddTab $p $this::TabWidget2 $this::tab2 "Radio + Check + Edit"

    pvQButtonGroup $p $this::ButtonGroup1 $this::tab2 -1 $::HORIZONTAL "ButtonGroup1"
    pvSetGeometry $p $this::ButtonGroup1 20 20 151 140

    pvQRadioButton $p $this::RadioButton3 $this::ButtonGroup1
    pvSetGeometry $p $this::RadioButton3 10 100 131 31
    pvSetText $p $this::RadioButton3 "RadioButton3"

    pvQRadioButton $p $this::RadioButton2 $this::ButtonGroup1
    pvSetGeometry $p $this::RadioButton2 10 60 131 31
    pvSetText $p $this::RadioButton2 "RadioButton2"

    pvQRadioButton $p $this::RadioButton1 $this::ButtonGroup1
    pvSetGeometry $p $this::RadioButton1 10 20 131 31
    pvSetText $p $this::RadioButton1 "RadioButton1"
    pvSetChecked $p $this::RadioButton1 1

    pvQButtonGroup $p $this::ButtonGroup2 $this::tab2 -1 $::HORIZONTAL "ButtonGroup2"
    pvSetGeometry $p $this::ButtonGroup2 20 170 150 140

    pvQCheckBox $p $this::CheckBox3 $this::ButtonGroup2
    pvSetGeometry $p $this::CheckBox3 10 100 121 31
    pvSetText $p $this::CheckBox3 "CheckBox3"

    pvQCheckBox $p $this::CheckBox2 $this::ButtonGroup2
    pvSetGeometry $p $this::CheckBox2 10 60 121 31
    pvSetText $p $this::CheckBox2 "CheckBox2"

    pvQCheckBox $p $this::CheckBox1 $this::ButtonGroup2
    pvSetGeometry $p $this::CheckBox1 10 20 121 31
    pvSetText $p $this::CheckBox1 "CheckBox1"
    pvSetChecked $p $this::CheckBox1 1

    pvQPushButton $p $this::PushButtonRequestMultiLineEdit $this::tab2
    pvSetGeometry $p $this::PushButtonRequestMultiLineEdit 18 315 151 31
    pvSetText $p $this::PushButtonRequestMultiLineEdit "Request MultiLineEdit"

    pvQMultiLineEdit $p $this::MultiLineEdit1 $this::tab2 0 30
    pvSetGeometry $p $this::MultiLineEdit1 180 10 491 411

    pvQWidget $p $this::tab3 $this::TabWidget2
    pvAddTab $p $this::TabWidget2 $this::tab3 "Edit + Combo + Slider + Browser"

    pvQLabel $p $this::TextLabel3 $this::tab3
    pvSetGeometry $p $this::TextLabel3 168 15 101 31
    pvSetText $p $this::TextLabel3 "Edit"

    pvQComboBox $p $this::ComboBox1 $this::tab3 0 $::AtTop
    pvSetGeometry $p $this::ComboBox1 18 55 141 31

    pvQLabel $p $this::TextLabel4 $this::tab3
    pvSetGeometry $p $this::TextLabel4 168 55 101 31
    pvSetText $p $this::TextLabel4 "Combo"

    pvQLabel $p $this::TextLabel5 $this::tab3
    pvSetGeometry $p $this::TextLabel5 168 95 101 31
    pvSetText $p $this::TextLabel5 "Combo editable"

    pvQComboBox $p $this::ComboBox2 $this::tab3 1 $::AtTop
    pvSetGeometry $p $this::ComboBox2 18 95 141 31

    pvQSlider $p $this::Slider1 $this::tab3 0 99 10 0 $::Horizontal
    pvSetGeometry $p $this::Slider1 18 145 141 31

    pvQLabel $p $this::TextLabel6 $this::tab3
    pvSetGeometry $p $this::TextLabel6 168 145 101 31
    pvSetText $p $this::TextLabel6 "Slider"

    pvQSlider $p $this::Slider2 $this::tab3 0 99 10 0 $::Vertical
    pvSetGeometry $p $this::Slider2 18 185 31 141

    pvQPushButton $p $this::PushButtonBackward $this::tab3
    pvSetGeometry $p $this::PushButtonBackward 330 10 50 20
    pvSetText $p $this::PushButtonBackward "<-"

    pvQPushButton $p $this::PushButtonForward $this::tab3
    pvSetGeometry $p $this::PushButtonForward 380 10 50 20
    pvSetText $p $this::PushButtonForward "->"

    pvQTextBrowser $p $this::TextBrowser1 $this::tab3
    pvSetGeometry $p $this::TextBrowser1 278 36 381 380

    pvQPushButton $p $this::PushButtonHome $this::tab3
    pvSetGeometry $p $this::PushButtonHome 280 10 50 20
    pvSetText $p $this::PushButtonHome "Home"

    pvQLineEdit $p $this::LineEdit1 $this::tab3
    pvSetGeometry $p $this::LineEdit1 18 15 141 31
    pvSetText $p $this::LineEdit1 ""
    pvSetEchoMode $p $this::LineEdit1 2

    pvQWidget $p $this::tab4 $this::TabWidget2
    pvAddTab $p $this::TabWidget2 $this::tab4 "Image + Draw + Frames"

    pvQDraw $p $this::Draw1 $this::tab4
    pvSetGeometry $p $this::Draw1 298 5 371 280

    pvQLabel $p $this::TextLabel2_2 $this::tab4
    pvSetGeometry $p $this::TextLabel2_2 20 300 300 31
    pvSetText $p $this::TextLabel2_2 "a JPG Image (you can also use it as a Webcam)"

    pvQLabel $p $this::TextLabel1_2 $this::tab4
    pvSetGeometry $p $this::TextLabel1_2 390 300 151 31
    pvSetText $p $this::TextLabel1_2 "a DrawImage"

    pvQFrame $p $this::Frame3 $this::tab4 $::StyledPanel $::Raised 5 0
    pvSetGeometry $p $this::Frame3 8 5 278 278

    pvDownloadFile $p "ajpg.jpg"
    pvQImage $p $this::Image1 $this::Frame3 "ajpg.jpg" $w $h $depth
    pvSetGeometry $p $this::Image1 10 10 256 256

    pvQLabel $p $this::PixmapLabel1 $this::tab4
    pvSetGeometry $p $this::PixmapLabel1 610 370 32 32
    pvSetText $p $this::PixmapLabel1 ""

    pvQWidget $p $this::tab5 $this::TabWidget2
    pvAddTab $p $this::TabWidget2 $this::tab5 "ListView + IconView"

    pvQIconView $p $this::IconView1 $this::tab5
    pvSetGeometry $p $this::IconView1 338 15 331 401

    pvQPushButton $p $this::PushButtonRequestListView $this::tab5
    pvSetGeometry $p $this::PushButtonRequestListView 10 380 161 31
    pvSetText $p $this::PushButtonRequestListView "Request Selection"

    pvQListView $p $this::ListView1 $this::tab5
    pvSetGeometry $p $this::ListView1 8 16 321 360
    pvSetMultiSelection $p $this::ListView1 1

    pvQWidget $p $this::tab6 $this::TabWidget2
    pvAddTab $p $this::TabWidget2 $this::tab6 "VTK"

    pvQVtkTclWidget $p $this::VtkTclWidget1 $this::tab6
    pvSetGeometry $p $this::VtkTclWidget1 8 4 531 421

    pvQSlider $p $this::VtkSlider1 $this::tab6 0 99 10 50 $::Vertical
    pvSetGeometry $p $this::VtkSlider1 548 4 31 141

    pvQSlider $p $this::VtkSlider2 $this::tab6 0 99 10 50 $::Horizontal
    pvSetGeometry $p $this::VtkSlider2 548 154 121 31

    pvEndDefinition $p
    delete_int $w
    delete_int $h
    delete_int $depth
    return 0
  }

  # _end_of_generated_area_ (do not edit -> use ui2pvc) ---------------

  proc defineMask { p } {
    generated_defineMask $p
    # (todo: add your code here)
    return 0
  }

  proc showData { p } {
    # (todo: add your code here)
    return 0
  }

  proc readData { p } { # from shared memory, database or something else
    # (todo: add your code here)
    return 0
  }

  proc show { p } {
    set text  ""
    set event ""
    set str1  ""
    set val    0
    set x      0
    set y      0
    set button 0
    defineMask $p
    # from shared memory, database or something else
    readData $p
    showData $p
    while { 1 } {
      set event [ pvGetEvent $p ]
      set result [ pvParseEventStruct $p $event ]
      set id   [ $result cget -event ]
      set i    [ $result cget -i ]
      set text [ $result cget -text ]
      if       { $id == $::NULL_EVENT } {
        # from shared memory, database or something else
        readData $p
        showData $p
      } elseif { $id == $::BUTTON_EVENT } {
        puts [ format "BUTTON_EVENT id=%d" $i ]
      } elseif { $id == $::BUTTON_PRESSED_EVENT } {
        puts [ format "BUTTON_PRESSED_EVENT id=%d" $i ]
      } elseif { $id == $::BUTTON_RELEASED_EVENT } {
        puts [ format "BUTTON_RELEASED_EVENT id=%d" $i ]
      } elseif { $id == $::TEXT_EVENT } {
        puts [ format "TEXT_EVENT id=%d text=%s" $i $text ]
      } elseif { $id == $::SLIDER_EVENT } {
        scan $text "(%d)" val
        puts [ format "SLIDER_EVENT val=%d" $val ]
      } elseif { $id == $::CHECKBOX_EVENT } {
        puts [ format "CHECKBOX_EVENT id=%d text=%s" $i $text ]
      } elseif { $id == $::RADIOBUTTON_EVENT } {
        puts [ format "RADIOBUTTON_EVENT id=%d text=%s" $i $text ]
      } elseif { $id == $::GL_INITIALIZE_EVENT } {
        puts "you have to call initializeGL()"
      } elseif { $id == $::GL_PAINT_EVENT } {
        puts "you have to call paintGL()"
      } elseif { $id == $::GL_RESIZE_EVENT } {
        scan $text "(%d,%d)" w h
        puts [ format "you have to call resizeGL(%d,%d)" $w $h ]
      } elseif { $id == $::GL_IDLE_EVENT } {
        puts "glIdle()"
      } elseif { $id == $::TAB_EVENT } {
        scan $text "(%d)" val
        puts [ format "TAB_EVENT i=%d val=%d" $i $val ]
      } elseif { $id == $::TABLE_TEXT_EVENT } {
        scan $text "(%d,%d)" x y
        set str1 [ pvGetTextParam $p $text ]
        puts [ format "TABLE_TEXT_EVENT x=%d y=%d str=%s" $x $y $str1 ]
      } elseif { $id == $::TABLE_CLICKED_EVENT } {
        scan $text "(%d,%d,%d)" x y button
        puts [ format "TABLE_CLICKED_EVENT x=%d y=%d button=%d" $x $y $button ]
      } elseif { $id == $::SELECTION_EVENT } {
        scan $text "(%d)" val
        set str1 [ pvGetTextParam $p $text ]
        puts [ format "SELECTION_EVENT(column=%d str1=%s" $val $str1 ]
      } elseif { $id == $::CLIPBOARD_EVENT } {
        scan $text "(%d)" val
        puts [ format "CLIPBOARD_EVENT id=%d" $val ]
        puts [ format "clipboard = %s" [ $p cget -clipboard ] ]
      } elseif { $id == $::RIGHT_MOUSE_EVENT } {
        puts [ format "RIGHT_MOUSE_EVENT id=%d" $i ]
        # pvPopupMenu $p -1 "Menu1,Menu2,,Menu3"
      } elseif { $id == $::KEYBOARD_EVENT } {
        scan $text "(%d)" val
        puts [ format "KEYBOARD_EVENT modifier=%d val=%d" $i $val ]
      } else {
        puts [ format "UNKNOWN_EVENT id=%d text=%s" $i $text ]
      }
    }
  }
}

set ret [ periodic::show $p ]
return $ret
