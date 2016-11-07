/* 
 * File:   SuperCon-badge-animate.c
 * Author: tyler@holmesengineering.com
 *
 * MIT License (see license.txt)
 * Copyright (c) 2016 Hackaday.com
 */

#include "HaD_Badge.h"
#include "binary-clock.h"

//! private admin struct to keep track of the time
typedef struct {
    uint8_t hour_ten;
    uint8_t hour_one;
    uint8_t min_ten;
    uint8_t min_one;
    uint8_t sec_ten;
    uint8_t sec_one;
} clock_t;

//! structure to keep track of the set state
typedef enum {
    HOUR_TEN,
    HOUR_ONE,
    MIN_TEN,
    MIN_ONE,
    SEC_TEN,
    SEC_ONE,
} set_state_t;

//! structure for the main clock state
typedef enum {
    RUN,
    SET,
}clock_state_t;

// private state variables
clock_state_t clock_state = RUN;  //!< keeps track of the main clock state (RUN/SET)
set_state_t set_state = HOUR_TEN; //!< sub-state tracker of the main clock set state

// private global variables
clock_t clock;           //!< clock admin structure holding the time
clock_t *clock_ptr;      //!< pointer to the clock struct
uint32_t previous_time;  //!< keeps track of the last time we inc'd the clock

// private functions
void init_clock(void);
void set_time(uint8_t hour, uint8_t minute, uint8_t second);
void update_clock(clock_state_t state);
void increment_time(uint8_t delta);
void update_display(void);
void set_row(uint8_t row, uint16_t data);
uint8_t getRandomNumber(void);
void run_set(void);



uint8_t getRandomNumber(void) {
    asm("goto 0x2B04"); // Function address for random number generator
    return WR;
}


/*!
 * Increments the time by polling `getTime()` and adjusts the brightness by
 * checking the accelerometer y axis
 */
void init_clock(void)
{
    clock_ptr = &clock;
    set_time(12, 30, 00);
    previous_time = getTime();
    Brightness = 15;
    PowerOFF = 255;  // powers off after 6*255 seconds (25 minutes)
}


/*!
 * Increments the time by polling `getTime()` and adjusts the brightness by
 * checking the accelerometer y axis
 * 
 * @param[in] hour (uint8_t): hours value to be set
 * @param[in] minute (uint8_t): minutes value to be set
 * @param[in] second (uint8_t): seconds value to be set
 */
void set_time(uint8_t hour, uint8_t minute, uint8_t second)
{
    clock_ptr->hour_ten = hour / 10;
    clock_ptr->hour_one = hour % 10;
    clock_ptr->min_ten = minute / 10;
    clock_ptr->min_one = minute % 10;
    clock_ptr->sec_ten = second / 10;
    clock_ptr->sec_one = second % 10;
}



void run_clock(void)
{
    init_clock();
    while(1) {
        update_clock(clock_state);
        controlDelayMs(50);
        switch(clock_state) {
            case RUN:
                if (getControl() == DOWN) {
                    clock_state = SET;
                }
                break;
            
            case SET:
                run_set();
            break;
        }
    }
}

/*!
 * Runs the set_state state machine. Loops through the columns, incrementing or
 * decrementing the time at each column.
 */
void run_set(void)
{
    uint8_t button;
    switch(set_state) {
        case HOUR_TEN:
            button = getControl();
            if (button == DOWN) {
                set_state = HOUR_TEN;
                clock_state = RUN;
            } else if (button == UP) {
                set_state = HOUR_ONE;
            } else if (button == LEFT) {
                // up for us
                if (clock.hour_ten < 2) {
                    clock.hour_ten += 1;
                } else {
                    clock.hour_ten -= 2;
                }
            } else if (button == RIGHT) {
                // down for us
                if (clock.hour_ten > 0) {
                    clock.hour_ten -= 1;
                } else {
                    clock.hour_ten = 2;
                }
            }
        break;

        case HOUR_ONE:
            button = getControl();
            if (button == DOWN) {
                set_state = HOUR_TEN;
                clock_state = RUN;
            } else if (button == UP) {
                set_state = MIN_TEN;
            } else if (button == LEFT) {
                // up for us
                if (clock.hour_ten == 2) {
                    if (clock.hour_one < 3) {
                        clock.hour_one += 1;
                    } else {
                        clock.hour_one = 0;
                    }
                } else {
                    if (clock.hour_one < 9) {
                        clock.hour_one += 1;
                    } else {
                        clock.hour_one = 0;
                    }
                }
            } else if (button == RIGHT) {
                // down for us
                if (clock.hour_ten == 2) {
                    if (clock.hour_one > 0) {
                        clock.hour_one -= 1;
                    } else {
                        clock.hour_one = 3;
                    }
                } else {
                    if (clock.hour_one > 0) {
                        clock.hour_one -= 1;
                    } else {
                        clock.hour_one = 9;
                    }
                }
            }
        break;

        case MIN_TEN:
            button = getControl();
            if (button == DOWN) {
                set_state = HOUR_TEN;
                clock_state = RUN;
            } else if (button == UP) {
                set_state = MIN_ONE;
            } else if (button == LEFT) {
                // up for us
                if (clock.min_ten < 5) {
                    clock.min_ten += 1;
                } else {
                    clock.min_ten -= 5;
                }
            } else if (button == RIGHT) {
                // down for us
                if (clock.min_ten > 0) {
                    clock.min_ten -= 1;
                } else {
                    clock.min_ten = 5;
                }
            }
        break;

        case MIN_ONE:
            button = getControl();
            if (button == DOWN) {
                set_state = HOUR_TEN;
                clock_state = RUN;
            } else if (button == UP) {
                set_state = SEC_TEN;
            } else if (button == LEFT) {
                // up for us
                if (clock.min_one < 9) {
                    clock.min_one += 1;
                } else {
                    clock.min_one = 0;
                }
            } else if (button == RIGHT) {
                // down for us
                if (clock.min_one > 0) {
                    clock.min_one -= 1;
                } else {
                    clock.min_one = 9;
                }
            }
        break;

        case SEC_TEN:
            button = getControl();
            if (button == DOWN) {
                set_state = HOUR_TEN;
                clock_state = RUN;
            } else if (button == UP) {
                set_state = SEC_ONE;
            } else if (button == LEFT) {
                // up for us
                if (clock.sec_ten < 5) {
                    clock.sec_ten += 1;
                } else {
                    clock.sec_ten -= 5;
                }
            } else if (button == RIGHT) {
                // down for us
                if (clock.sec_ten > 0) {
                    clock.sec_ten -= 1;
                } else {
                    clock.sec_ten = 5;
                }
            }
        break;

        case SEC_ONE:
            button = getControl();
            if (button == DOWN) {
                set_state = HOUR_TEN;
                clock_state = RUN;
            } else if (button == UP) {
                set_state = HOUR_TEN;
            } else if (button == LEFT) {
                // up for us
                if (clock.sec_one < 9) {
                    clock.sec_one += 1;
                } else {
                    clock.sec_one = 0;
                }
            } else if (button == RIGHT) {
                // down for us
                if (clock.sec_one > 0) {
                    clock.sec_one -= 1;
                } else {
                    clock.sec_one = 9;
                }
            }
        break;
    }
}


/*!
 * Increments the time by polling `getTime()` and adjusts the brightness by
 * checking the accelerometer y axis
 * 
 * @param[in] state (clock_state_t): current state of the clock. i.e. RUN for
 *      the normal time incrementing and displaying the normal clock face or
 *      SET for blinking the time
 */
void update_clock(clock_state_t state)
{
    uint32_t current_time = getTime();
    int16_t AccY;
    
    // periodically check for brightness update
    if (current_time - previous_time > 300) {
        // update and process
        pollAccel();  // update accel vars
        // get a signed weighted representation of the accel in y direction.
        AccY = AccYlow - AccYhigh;
        
        // adjust brightness based on thresholds
        if (AccY < -500) {
            if (Brightness > 0) {
                Brightness -= 1;
            }
        }
        else if (AccY > 300) {
            if (Brightness < 15) {
                Brightness += 1;
            }
        }
        
        // if you flip the badge, generate random valid time
        if (AccY > 800 && clock_state == RUN) {
            uint8_t hour = 0;
            uint8_t minute = 0;
            uint8_t second = 0;
            // get a random number and normalize it a bit
            hour = getRandomNumber() >> 3;
            if (hour > 23) {
                hour = 23;
            }
            
            minute = getRandomNumber() >> 2;
            if (minute > 60) {
                minute = 59;
            }
            
            second = getRandomNumber() >> 2;
            if (second > 60) {
                second = 59;
            }
            set_time(hour, minute, second);
        }
    }
    
    // run main timing (1800 counts ~= 1 second)
    if (current_time - previous_time > 1800) {  // timing adjustments
        // run the time incrementing or setting
        if (state == RUN) {
            // update time variables
            increment_time(1);
            previous_time = current_time;
            // display the new time
            update_display();
        } else {
            // setting time. Blink the screen
            if (current_time % 2000 >= 600) {    // timing adjustments
                update_display();
            } else {
                displayClear();  // Turn all LEDs off
                // keep row that is being edited on
                switch(set_state) {
                    case HOUR_TEN:
                        set_row(HOUR_TEN_ROW, clock_ptr->hour_ten);
                    break;

                    case HOUR_ONE:
                        set_row(HOUR_ONE_ROW, clock_ptr->hour_one);
                    break;

                    case MIN_TEN:
                        set_row(MIN_TEN_ROW, clock_ptr->min_ten);
                    break;

                    case MIN_ONE:
                        set_row(MIN_ONE_ROW, clock_ptr->min_one);
                    break;

                    case SEC_TEN:
                        set_row(SEC_TEN_ROW, clock_ptr->sec_ten);
                    break;

                    case SEC_ONE:
                        set_row(SEC_ONE_ROW, clock_ptr->sec_one);
                    break;
                }
                displayLatch();
            }
        }
    }
}


/*!
 * Updates the time in the clock struct. Pretty ugly unfortunately :'(
 * 
 * @param[in] delta (uint8_t): amount of seconds to increment by.
 */
void increment_time(uint8_t delta)
{
    // handle seconds...
    clock_ptr->sec_one += delta;
    if (clock_ptr->sec_one >= 10) {
        clock_ptr->sec_one -= 10;
        clock_ptr->sec_ten += 1;
        if (clock_ptr->sec_ten >= 6) {
            clock_ptr->sec_ten -= 6;
            
            // handle minutes...
            clock_ptr->min_one += 1;
            if (clock_ptr->min_one >= 10) {
                clock_ptr->min_one -= 10;
                clock_ptr->min_ten += 1;
                if (clock_ptr->min_ten >= 6) {
                    clock_ptr->min_ten -= 6;
                    
                    // handle hours...
                    clock_ptr->hour_one += 1;
                    if (clock_ptr->hour_ten == 1) {
                        if (clock_ptr->hour_one >= 10) {
                            clock_ptr->hour_one -= 10;
                            clock_ptr->hour_ten += 1; 
                        }
                    } else if (clock_ptr->hour_ten == 2) {
                        if (clock_ptr->hour_one >= 3) {
                            clock_ptr->hour_one = 0;
                            clock_ptr->hour_ten = 0; 
                        }
                    }
                }
            }
        }
    }
}


/*!
 * Updates the display according to the clock pointer.
 */
void update_display(void)
{
    // go through and update each clock "row"
    set_row(HOUR_TEN_ROW, clock_ptr->hour_ten);
    set_row(HOUR_ONE_ROW, clock_ptr->hour_one);
    set_row(MIN_TEN_ROW, clock_ptr->min_ten);
    set_row(MIN_ONE_ROW, clock_ptr->min_one);
    set_row(SEC_TEN_ROW, clock_ptr->sec_ten);
    set_row(SEC_ONE_ROW, clock_ptr->sec_one);

    // display data
    displayLatch();
}


/*!
 * Sets a given row (which is two pixels wide) with the first four bits of
 * the given data.
 * 
 * @param[in] row (uint8_t): the row index that you want to update
 * @param[in] data (uint8_t): the data you want to display (the lsb nibble)
 */
void set_row(uint8_t row, uint16_t data)
{
    // build up the data string
    uint16_t new_data = 0;
    for(int i = 0; i < 4; i += 1) {
        if((1 << i) & data) {
            new_data |= (0b11 << (2*i));
        }
    }
    // set the bits)
    for (int i = 0; i < 2; i++) {
        // set display buffer to be displayed
        Buffer[i+row] = new_data & 0xFF;
    }
}
