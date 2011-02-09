-------------------------------------------------------------------------------------
-- pvapplication in lua       run: pvapplua
-------------------------------------------------------------------------------------
                              -- here you may put variables global for all your masks
-- declare the data acquisition class for connecting to modbus
-- this class communicates with the modus_daemon via a shared memeory and a mailbox
--                                                Mailbox                         Shared Memory  ShmSize  
mb = rllib.rlDataAcquisition("/srv/automation/mbx/modbus1.mbx","/srv/automation/shm/modbus1.shm",65536)

inp = {}                  -- declare some memory for inputs
inp[1] = rllib.rlPlcMem()
inp[2] = rllib.rlPlcMem()

a_val = 0

-------------------------------------------------------------------------------------
function readInputs()
  inp[1].i_old = inp[1].i  -- read new input values from modbus slave=1
  inp[2].i_old = inp[2].i  -- inp may be used in all slot functions
  inp[1].i = mb.intValue(mb,"coilStatus(1,0)")
  inp[2].i = mb.intValue(mb,"coilStatus(1,8)")
end

-------------------------------------------------------------------------------------
function step1()
  if a_val > 10 then
    print("step1 inp[1].i=", inp[1].i, " reset coil")
    mb.writeIntValue(mb,"coil(1,0)",0)
    a_val = 0
  elseif a_val <= 1 then
    print("step1 inp[1].i=", inp[1].i, " set coil")
    mb.writeIntValue(mb,"coil(1,0)",1)
  else
    print("step1 inp[1].i=", inp[1].i)
  end  
  a_val = a_val + 1
end

-------------------------------------------------------------------------------------
function step2()
  print("step2 inp[2].i=", inp[2].i)
end

-------------------------------------------------------------------------------------
function luaMain(trace)       -- Lua Main Program

  print("Lua is here trace = ", trace)
  print("You might want to implement a softPLC in Lua using this")

  while true do
    readInputs()
    step1()
    step2()
    rllib.rlsleep(500)
  end

  return 0

end
