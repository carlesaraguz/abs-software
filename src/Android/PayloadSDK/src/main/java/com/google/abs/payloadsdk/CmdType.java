package com.google.abs.payloadsdk;

public enum CmdType
{
    HANDSHAKE       (new byte[]{0, 0, 0}),
    DIGITAL_WRITE   (new byte[]{1, 0, 1}),
    DIGITAL_READ    (new byte[]{1, 0, 2}),
    ANALOG_WRITE    (new byte[]{1, 0, 3}),
    ANALOG_READ     (new byte[]{1, 0, 4}),
    WRITE_SERIAL    (new byte[]{1, 0, 5}),
    READ_SERIAl     (new byte[]{1, 0, 6}),
    OK              (new byte[]{(byte)253, 0, 0}),
    OK_DATA         (new byte[]{(byte)254, 0, 0}),
    ERROR           (new byte[]{(byte)255, 0, 0});

    private final byte[] cmd;

    CmdType(byte[] cmd)
    {
        this.cmd = cmd;
    }

    private byte[] cmd()
    {
        return cmd;
    }

    public byte[] getCmd()
    {
        return cmd;
    }
}