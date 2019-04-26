# washing-machine-status

This project will measure if the current is flowing in a power cord to the washing machine. If there is current for a period of time and then it stops, a text message will be sent to a cell number. 

Using this Library, at this link, it's possible to generate the current 
https://github.com/openenergymonitor/EmonLib/blob/master/examples/current_only/current_only.ino#L15

Here is a good website where 
https://learn.openenergymonitor.org/electricity-monitoring/ct-sensors/how-to-build-an-arduino-energy-monitor-measuring-current-only

Plan is 

1) - get board to measure current
2) - assign current a variable
3) - report that variable to thingspeak

