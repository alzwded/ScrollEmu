ScrollEmu
=========

Context: I have a logitech trackball with a horrible scroll wheel implementation -- two horrible scroll implementations to be exact.

So I've put this together which allows me to lean down on button #5 and then use the whole ball to send vertical _and_ horizontal scroll events (giant scroll wheel, yay!!)

Now that I'm getting ready to push this, I realized I've implemented natural scrolling by accident. Whoops. That will need to be fixed.

TODO:
* toggle between natural and traditional scrolling
* mappable "hotkey" to either X buttons or have a keyboard modifier like scroll lock/winkey

Read all about it
=================

Launch ScreenEmu.exe. It will ask to be elevated ~~and to get accessibility access~~. [1]

[1]: that raises issues with code signing and trust, unfortunately

You can now hold down "mouse extra button 2" and move the mouse; this will send vertical and horizontal scroll wheel movement instead of moving the pointer.

Scrolling is backwards ("natural") by mistake.

How does it work?
--------------------

This app installs a low level mouse hook that listens for mouse button X2 (usually like the 5th button or something like that). When it's depressed, it starts listening for mouse movement and replacing that with scroll events. When released, returns to step 1.

This app asks for elevation and ui automation in order to "work" in as many windows as possible. If it runs unelevated and not as an accessibility app, it will fail to intercept the mouse when a human is interacting with certain windows. It can still fail even with all this elevation, but less so.

Can't autohotkey already do this?
-------------------------------------

Short answer: yes.

What's next?
--------------

Add some options to help you pick which mouse button triggers this behaviour (middle, x1 or x2) or allow some kind of global modifier like the "scroll lock key" or "windows key" or "menu key" or "caps lock" or whatever to act as the magical key to enter "scroll mode". I'll get to that eventually.
