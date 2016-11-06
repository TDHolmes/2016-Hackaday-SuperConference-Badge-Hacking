/* 
 * File:   SuperCon-badge-animate.c
 * Author: tyler@holmesengineering.com
 *
 * MIT License (see license.txt)
 * Copyright (c) 2016 Hackaday.com
 */

#include "HaD_Badge.h"
#include "binary-clock.h"

// private admin struct
typedef struct {
    uint8_t hour_ten;
    uint8_t hour_one;
    uint8_t min_ten;
    uint8_t min_one;
    uint8_t sec_ten;
    uint8_t sec_one;
} clock_t;


typedef enum {
    HOUR_TEN,
    HOUR_ONE,
    MIN_TEN,
    MIN_ONE,
    SEC_TEN,
    SEC_ONE,
} set_state_t;

typedef enum {
    RUN,
    SET,
}clock_state_t;

clock_state_t clock_state = RUN;
set_state_t set_state = HOUR_TEN;

// private global variables
clock_t clock;
clock_t *clock_ptr;
uint32_t previous_time;

// private functions
void init_clock(void);
void set_time(uint8_t hour, uint8_t minute, uint8_t second);
void update_clock(clock_state_t state);
void increment_time(uint8_t delta);
void update_display(void);
void set_row(uint8_t row, uint16_t data);



void init_clock(void)
{
    clock_ptr = &clock;
    set_time(12, 30, 00);
    previous_time = getTime();
}

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
        uint8_t button;
        update_clock(clock_state);
//        controlDelayMs(50);
        switch(clock_state) {
            case RUN:
                if (getControl() == DOWN) {
                    clock_state = SET;
                }
                break;
            
            case SET:
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
                            if (clock.hour_ten > 1) {
                                clock.hour_ten -= 1;
                            } else {
                                clock.hour_ten += 2;
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
                            if (clock.min_ten > 1) {
                                clock.min_ten -= 1;
                            } else {
                                clock.min_ten += 5;
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
                                clock.min_one += 9;
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
                            if (clock.sec_ten > 1) {
                                clock.sec_ten -= 1;
                            } else {
                                clock.sec_ten += 5;
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
                                clock.sec_one += 9;
                            }
                        }
                    break;
                }
            break;
        }
    }
}


void update_clock(clock_state_t state)
{
    uint32_t current_time = getTime();
    // check time
    if (current_time - previous_time > 1000) {
        if (state == RUN) {
            // update time variables
            increment_time(1);
            previous_time = current_time;
            
            update_display();
        } else {
            // setting time. Blink the screen
            if (current_time % 1000 >= 300) {
                update_display();
            } else {
                displayClear();  // Turn all LEDs off
                displayLatch();
            }
        }
    }
}


void increment_time(uint8_t delta)
{
    clock_ptr->sec_one += delta;
    if (clock_ptr->sec_one >= 10) {
        clock_ptr->sec_one -= 10;
        clock_ptr->sec_ten += 1;
        if (clock_ptr->sec_ten >= 6) {
            clock_ptr->sec_ten -= 6;
            clock_ptr->min_one += 1;
            if (clock_ptr->min_one >= 10) {
                clock_ptr->min_one -= 10;
                clock_ptr->min_ten += 1;
                if (clock_ptr->min_ten >= 6) {
                    clock_ptr->min_ten -= 6;
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


void update_display(void)
{
    // clear not necessary because set row clears or sets all bits every time
    // displayClear();  // Turn all LEDs off
    set_row(HOUR_TEN_ROW, clock_ptr->hour_ten);
    set_row(HOUR_ONE_ROW, clock_ptr->hour_one);
    set_row(MIN_TEN_ROW, clock_ptr->min_ten);
    set_row(MIN_ONE_ROW, clock_ptr->min_one);
    set_row(SEC_TEN_ROW, clock_ptr->sec_ten);
    set_row(SEC_ONE_ROW, clock_ptr->sec_one);
    displayLatch();
}


void set_row(uint8_t row, uint16_t data)
{
    // build up the data string
    uint16_t new_data = 0;
    for(int i = 0; i < 4; i += 1) {
        if((1 << i) & data) {
            new_data |= (0b11 << (2*i));
        }
    }
    // set the bits
    for (int i = 0; i < 2; i++) {
        // set display buffer to be displayed
        Buffer[i+row] = new_data & 0xFF;
    }
}
