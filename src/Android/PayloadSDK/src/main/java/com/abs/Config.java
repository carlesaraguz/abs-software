package com.abs;

public enum Config
{
    DIGITAL_WRITE   (101),
    DIGITAL_READ    (102),
    ANALOG_WRITE    (103),
    ANALOG_READ     (104),
    WRITE_I2C       (105),
    READ_I2C        (106),
    WRITE_UART      (107),
    READ_UART       (108);

    private final int cmd;

    Config(int cmd)
    {
        this.cmd = cmd;
    }

    private int cmd(){return cmd; }
}