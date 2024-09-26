			AREA Project, CODE, READONLY

gameData 	DCB 0x10, 0x02, 0x20, 0x01, 0x04, 0x01, 0x10, 0x02, 0x04, 0x20, 0x01, 0x10
DataLen 	EQU 12

			EXPORT __main
__main		PROC
			
			; GPIO REGISTERS
			; R0 is port 5 base address for OUTPUT (LED)
			; R1 is port 4 base address for INPUT (Button)
			; R2 is temp register for data processing
			; R7 is port 1 base address for OUTPUT (Timer)
			
			; DATA REGISTERS
			; R3 is level counter
			; R4 is pattern index
			; R5 is the pointer to game data
			; R6 is the input timer
			
			; FUNCTION REGISTERS
			; R8 is a function variable/timer temp address register
			; R9 is timer mode value/timer temp address register
			; R10 used to set frequency for timer
			
			
			; P5.0, P5.1, P5.2, P5.4, P5.5 are LED output
			; P5.6 is buzzer output
			
			; P4.0, P4.1, P4.2, P4.4, P4.5 are corresponding LED buttons
			; P4.6 is game start button
			
			; Configure GPIO on port 5
			LDR R0, =0x40004C40 	; Port 5 base address
			
			; Configure GPIO on port 4
			LDR R1, =0x40004C21		; Port 4 base address
			
			; Configure GPIO timer on port 2
			LDR R7, =0x40004C01		; Port 2 base address
			
			; Configure output pins
			MOV R2, #0x10 			; Byte to configure pins
			STRB R2, [R7, #0x04]	; Set as output pin
			MOV R2, #0x77 			; Byte to configure pins
			STRB R2, [R0, #0x04]	; Set as output pins
			
			; Configure input pins with pull up
			STRB R2, [R1, #0x06]	; Set as input pin
			STRB R2, [R1, #0x0E]	; Set pull down
			
			; Initialize the timer on port 1
			BL timerInit
			
			; Body instructions for 'Repeat After Me'

			; Load P4.6 to check when game should start
gameWait 	LDRB R2, [R1, #0x00] 	; Load input register value to R2
			AND R2, #0x40			; Mask P4.6
			CMP R2, #0x40			; Ensures start button pressed	
			
			MOV R2, #0x37			; Byte to select all LED
			STRB R2, [R0, #0x02]	; Turn LED ON
			
			BNE gameWait			
			
			BL shortDelay			; Remain ON for a second
			
			MOV R2, #0x37			; Byte to select pin 1
			STRB R2, [R0, #0x02]	; Turn LEDs OFF
			
			; Set initial pattern count
			MOV R3, #2				; Total pattern count must be loaded for fresh game
			
			; Reset data pattern for each level
moreLevels	LDR R5, =gameData		; Reset the pointer to gameData
			MOV R4, #0				; Reset pattern item counter
			ADD R3, #1				; Increase the total items in pattern
			
			BL lightShow			; light show for beating a level and starting game
			
			; Show the pattern that must be followed
morePattern LDRB R2, [R5]			; load value from array
			STRB R2, [R0,#0x02]		; store value in output register
			
			BL buzz					; activate buzzer
			
			BL delay				; wait a second so player can memorize
			
			BL unbuzz				; deactivate buzzer
			
			ADD R5, #1				; increment pointer to data array
			ADD R4, #1				; increment for each pattern item
			CMP R3, R4				; ensure all items in pattern displayed for current level
			BNE morePattern			 
			
			; Reset data pattern to validate input
			LDR R5, =gameData		; Reset the pointer to gameData
			MOV R4, #0				; Reset pattern item counter	
			
			; Check player input
			B startInput			; dont increment counter yet
			
nextInput	ADD R5, #1				; increment pointer to data array
			ADD R4, #1				; increment for each pattern item
startInput	MOV R6, #0				; Counter for how long left to input
inputWait	ADD R6, #1				; Increment counter
			
			; If it takes too long game over
			CMP R6, #10				; check timer count
			BEQ loseGame			; lose game if response takes too long
			
			; Clear LED otherwise for new input
			MOV R2, #0x00			; set all bits low on output
			STRB R2, [R0,#0x02]		; store value in output register
			
			; Load player input
			LDRB R2, [R1, #0x00] 	; Load input register value to R1
			STRB R2, [R0,#0x02]		; store value in output register
			
			BL buzz					; activate buzzer
			
			BL delay				; pass a second to increment timer
			
			BL unbuzz				; deactivate buzzer
			
			; Verify player input
			;AND R2, R10 			; dont mask bit to ensure input is the same
			CMP R2, R5				; check if input is the same as pattern value
			BNE loseGame			; player will lose if input is different
			
			CMP R3, R4				; ensure all items in pattern inputted for current level
			BNE nextInput			
			
			CMP R3, #DataLen		; increment the pattern limit if not not
			BNE moreLevels			
			
			BL winFunc				; player wins the game
			
			B gameWait	 			; return to start of game

loseGame	BL loseFunc				; player loses the game

			B gameWait				; return to start of game
			
			ENDP					; End of procedure __main
				
			; Short delay function for LED show
shortDelay 	FUNCTION
	
			MOV R8, #0x2000			; Determines delay period
continue1	SUB R8, #0x01			; Decrement loop for delay
			CMP R8, #0x00			; Check when delay should end
			BNE continue1			; Continue until R8 is positive
			
			BX LR					; Return to address in LR
			ENDP					; End of procedure shortDelay

			; Delay function for pattern LEDS
delay 		FUNCTION
	
			MOV R8, #0x5000			; Determines delay period
continue2	SUB R8, #0x01			; Decrement loop for delay
			CMP R8, #0x00			; Check when delay should end
			BNE continue2			; Continue until R8 is positive
			
			BX LR					; Return to address in LR
			ENDP					; End of procedure delay

			; timerInit function to start the timer on correct port
timerInit 	FUNCTION
			
			; Configure Timer_A for port 1
			LDRB R10, [R7, #0x0A]	; Load SEL0 address
			ORR R10, R10, #0x02		; Assign port 2
			STRB R10, [R7, #0x0A]
			
			LDRB R10, [R9, #0x0C]	; Load SEL1 address
			AND R10, R10, #0x02		; Assign port 2
			STRB R10, [R7, #0x0C]
			
			; Set up Timer_A
			LDR R9, =0x40000002  	; Load Timer_A0 CCTL address
			MOV R10, #0x2000      	; Load mode value
			STR R10, [R9]        	; Store mode in Timer_A0 CCTL
			
			LDR R9, =0x40000000  	; Load Timer_A0 CTL address
			MOV R10, #0x0204  		; Load the value to stop, clear timer and set clock source
			STR R10, [R9]      		; Store value in Timer_A0 CTL
			
			BX LR					; Return to address in LR
			ENDP					; End of procedure buzz

			; Buzz function to activate piezo buzzer
buzz	 	FUNCTION
			
			; Configure Timer_A for different frequencies
			LDR R9, =0x40000012  	; Load Timer_A0 CCR address
			MOV R10, #0xD48
			ADD R10, R10, R2, LSL #2 
			STR R10, [R9]        	; Store the frequency divider in Timer_A0 CCR0
			
			; Start Timer_A in up mode
			LDR R9, =0x40000000  	; Load Timer_A0 CTL address
			MOV R10, #0x0210 		; Load the value to start Timer_A in up mode
			STR R10, [R9]           ; Store the value in Timer_A0 CTL
			
			BX LR					; Return to address in LR
			ENDP					; End of procedure buzz
				
			; unBuzz function to activate piezo buzzer
unbuzz	 	FUNCTION
			
			; Stop Timer_A
			LDR R9, =0x40000000  	; Load Timer_A0 CTL address
			MOV R10, #0x0000 		; Load the value to stop Timer_A
			STR R10, [R9]           ; Store the value in Timer_A0 CTL
			
			BX LR					; Return to address in LR
			ENDP					; End of procedure buzz

			; Light show function for beginning of game
lightShow	FUNCTION
			
			; Light show from LED
			MOV R2, #0x01			; Byte to select pin 0
			STRB R2, [R0, #0x02]	; Turn LED ON
			BL shortDelay			; Remain ON for a second
			MOV R2, #0x02			; Byte to select pin 1
			STRB R2, [R0, #0x02]	; Turn LED ON
			BL shortDelay			; Remain ON for a second
			MOV R2, #0x04			; Byte to select pin 2
			STRB R2, [R0, #0x02]	; Turn LED ON
			BL shortDelay			; Remain ON for a second
			MOV R2, #0x08			; Byte to select pin 4
			STRB R2, [R0, #0x02]	; Turn LED ON
			BL shortDelay			; Remain ON for a second
			MOV R2, #0x10			; Byte to select pin 5
			STRB R2, [R0, #0x02]	; Turn LED ON
			BL shortDelay			; Remain ON for a second
			MOV R2, #0x00			; Byte to turn off pins
			STRB R2, [R0, #0x02]	; Turn LEDs OFF
			
			BX LR					; Return to address in LR
			ENDP					; End of procedure lightShow
				
			; Light show function for winning the game
winFunc		FUNCTION
	
			MOV R8, #3				; set counter
continue5	SUB R8, #1				; decrement counter
			
			; Light show from LED
			MOV R2, #0x04			; Byte to select pin 2
			STRB R2, [R0, #0x02]	; Turn LED ON
			BL shortDelay			; Remain ON for a second
			MOV R2, #0x0A			; Byte to select pins 1 and 3
			STRB R2, [R0, #0x02]	; Turn LED ON
			BL shortDelay			; Remain ON for a second
			MOV R2, #0x11			; Byte to select pins 0 and 5
			STRB R2, [R0, #0x02]	; Turn LED ON
			BL shortDelay			; Remain ON for a second
			MOV R2, #0x0A			; Byte to select pins 1 and 3
			STRB R2, [R0, #0x02]	; Turn LED ON
			BL shortDelay			; Remain ON for a second
			
			CMP R8, #0x00			; check counter
			BNE continue5			; repeat three times
			
			MOV R2, #0x04			; Byte to select pin 3
			STRB R2, [R0, #0x02]	; Turn LED ON
			BL shortDelay			; Remain ON for a second
			MOV R2, #0x00			; Byte to select no pins
			STRB R2, [R0, #0x02]	; Turn LEDs OFF
			
			BX LR					; Return to address in LR
			ENDP					; End of procedure winFunc
				
			; Light show function for losing the game
loseFunc	FUNCTION
	
			MOV R8, #3				; set counter
continue6	SUB R8, #1				; decrement counter
			
			; Light show from LED
			MOV R2, #0x37			; Byte to select all LEDs
			STRB R2, [R0, #0x02]	; Turn LED ON
			BL delay				; Remain ON for a second
			MOV R2, #0x00			; Byte to select no pins
			STRB R2, [R0, #0x02]	; Turn LED OFF
			
			CMP R8, #0				; check counter
			BNE continue6			; repeat three times
			
			BX LR					; Return to address in LR
			ENDP					; End of procedure loseFunc

			END