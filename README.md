# KnockLock
A knock lock is a device that unlocks a door lock in response to a sequence of knocks on the door. 
This project is the code to run an arduino-based knock lock, with the follow features:
- Define an arbitrary sequence of knocks (default is 3,2,1)
- Play a failure tone when the lock sequence fails (assumes a piezo buzzer is being used as a knock detector)
- Play a success tone when the lock sequence succeeds

There are many projects online that will tell you how to put together the hardware side of the knock lock.
