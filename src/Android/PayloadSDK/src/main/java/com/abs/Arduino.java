package com.abs;

import com.abs.sdb.SDB;
import com.abs.sdb.SDBPacket;

import java.nio.ByteBuffer;

public class Arduino {

    private SDB sdb;

    /**
     * Arduino constructor
     * @param sdb
     */

    public Arduino(SDB sdb)
    {
        this.sdb = sdb;
    }

    /**
     *
     * @param pin
     * @return
     */

    public byte[] digitalWrite(int pin)
    {
        SDBPacket packet = new SDBPacket(Config.DIGITAL_WRITE,(byte)1);
        return sdb.send(packet);
    }

    /**
     *
     * @param pin
     * @return
     */

    public byte[] digitalRead(int pin)
    {
        SDBPacket packet = new SDBPacket(Config.DIGITAL_WRITE,(byte)1);
        return sdb.send(packet);
    }

    /**
     *
     * @param pin
     * @return
     */

    public byte[] analogRead(int pin)
    {
        SDBPacket packet = new SDBPacket(Config.ANALOG_READ,(byte)1);
        return sdb.send(packet);
    }
}
