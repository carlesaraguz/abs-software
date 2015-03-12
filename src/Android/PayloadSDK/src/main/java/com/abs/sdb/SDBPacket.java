package com.abs.sdb;

import com.abs.Config;

import java.util.Arrays;

public class SDBPacket {

    private Config cmd;
    private byte[] parameters;

    /**
     * SDBPacket constructor
     * @param cmd
     */

    public SDBPacket(Config cmd)
    {
        this.cmd = cmd;
    }

    /**
     * SDBPacket constructor
     * @param cmd
     * @param param
     */

    public SDBPacket(Config cmd, byte... param)
    {
        this.cmd = cmd;
        this.parameters = param;
    }

    /**
     *
     * @return
     */

    public byte toRaw()
    {
        return 10;
    }

    /**
     *
     * @param param
     */

    public void setParameter(byte param)
    {
        add_parameter(param);
    }

    /**
     *
     * @param pos
     * @return
     */

    public int getParameter(int pos)
    {
        return parameters[pos];
    }

    /**
     *
     * @param element
     */

    private void add_parameter(byte element)
    {
        parameters = Arrays.copyOf(parameters, parameters.length + 1);
        parameters[parameters.length - 1] = element;
    }
}
