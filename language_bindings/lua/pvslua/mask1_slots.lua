-------------------------------------------------------------------------------------
-- mask1_slots.lua       Please edit this file in order to define your logic
-------------------------------------------------------------------------------------
                         -- here you may define variables local for your mask
                         -- also see the variables in the generated maskX.lua

function slotInit(p)     -- this function will be called before the event loop starts
  print("slotInit called")
  return 0
end

function slotNullEvent(p)
  print("slotNullEvent called")
  return 0
end

function slotButtonEvent(p,id)
  print("local button PushButtonBack=", PushButtonBack)
  return 0
end

function slotButtonPressedEvent(p,id)
  return 0
end

function slotButtonReleasedEvent(p,id)
  return 0
end

function slotTextEvent(p,id,text)
  return 0
end

function slotSliderEvent(p,id,val)
  return 0
end

function slotCheckboxEvent(p,id,text)
  return 0
end

function slotRadioButtonEvent(p,id,text)
  return 0
end

function slotGlInitializeEvent(p,id)
  return 0
end

function slotGlPaintEvent(p,id)
  return 0
end

function slotGlResizeEvent(p,id,width,height)
  return 0
end

function slotGlIdleEvent(p,id)
  return 0
end

function slotTabEvent(p,id,val)
  return 0
end

function slotTableTextEvent(p,id,x,y,text)
  return 0
end

function slotTableClickedEvent(p,id,x,y,button)
  return 0
end

function slotSelectionEvent(p,id,val,text)
  return 0
end

function slotClipboardEvent(p,id,val)
  return 0
end

function slotRightMouseEvent(p,id,text)
  return 0
end

function slotKeyboardEvent(p,id,val,modifier)
  return 0
end

function slotMouseMovedEvent(p,id,x,y)
  return 0
end

function slotMousePressedEvent(p,id,x,y)
  return 0
end

function slotMouseReleasedEvent(p,id,x,y)
  return 0
end

function slotMouseOverEvent(p,id,enter)
  return 0
end

function slotUserEvent(p,id,text)
  return 0
end
