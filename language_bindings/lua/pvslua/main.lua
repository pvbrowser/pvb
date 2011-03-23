-------------------------------------------------------------------------------------
-- pvserver in lua     run: pvslua -port=5050 -cd=/your/directory/with/your/lua/code
-------------------------------------------------------------------------------------
trace = 1              -- here you may put variables global for all your masks
                       -- example for loading a dll on windows
                       -- local fn,err = package.loadlib('./bit.dll','luaopen_bit')
dofile("mask1.lua")    -- include your masks here

-------------------------------------------------------------------------------------
function luaMain(ptr)  -- pvserver Lua Main Program

  p = pv.getParam(ptr) -- get the PARAM structure
  pv.pvSetCaption(p,string.format("Hello Lua %d",123))
  pv.pvResize(p,0,1280,1024)
  pv.pvGetInitialMask(p)
  print("Inital mask = ", p.initial_mask)

  ret = 1
  while 1 do           -- show your masks
    if     (ret==1) then
      ret = showMask1(p)
    else
      ret = 1
    end
  end

  pv.pvThreadFatal(p,"Lua calling ThreadFatal")
  return 0

end
