Hi folks:

I know it's been quite a while since I released anything new regarding this 
library.  Sorry about that - things have been a little hectic around here.
Let's just say that between keeping my (almost) 3 year old son occupied, 
rebuilding his room and roof (due to a major water leak), and changing 
jobs, I just haven't had much time for this.  I hope I can put more time 
into this and my other projects in the near future.  Anyhow...

Let's hear it for standards!  I found a major reason why I couldn't get 
data writing to the Pyramid working.  It seems that AB decided that 
non-byte data on the PLC5's should be little endian, but for some reason 
made the same non-byte data on the Pyramid big endian.  Of course, this is 
very important - such neat items like TNS, Count, Offset, etc are non-byte 
data (word length or 16 bits).  Once again, Ethereal (the packet sniffing 
software available from ethereal.zing.org) saves the day on this one.

Important to note that typed data writing to the pyramid is still broken.  
It's going to take a bit more to fix (and to discover what exactly is wrong 
with the data format that I am sending over).

This release is being released with 1 known bug right now.  When you do a 
data read of an I/O address (I:004), the library assumes you are sending 
the address over in decimal - not the octal that is conventional for a 
PLC5.  In other words, you will need to do a read of I:008, not I:010 as 
you would expect.  I'm looking into how to fix this - if anyone has a nice 
decimal to octal conversion routine they would like to donate!  :)

Lastly, I want to know from all of you folks - should I continue to provide 
support for the Pyramid?  Does anyone care about the Pyramid anymore?  Are 
you folks phasing out the pyramid in favor of a PC or a ControlLogix?  
Should I concentrate strictly on supporting the PLC5 and SLC5?  I need your 
opinions!

Oh, in case nobody noticed, I finally have my own domain.  FTP and WWW 
services are now hosted at my house.  Please be gentle on this - it's only 
a 56k dialup - but the cost was right!  A local provider is donating the 
line in exchange for services.  I'm not complaining!

Take care and stay in touch!

Ron Gage - Saginaw, MI
(ron@rongage.org)

P.S. - I am looking for contract computer work - preferably off-site.

