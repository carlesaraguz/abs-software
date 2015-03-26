package com.google.abs.payloadsdk.SBD;

import android.util.Log;
import com.google.abs.payloadsdk.CmdType;
import java.util.ArrayList;

public class SDBPacket {

    private CmdType cmd;
    private ArrayList<Byte> parameters;

    public SDBPacket(CmdType cmd)
    {
        this.cmd = cmd;
        Log.d("[ABS]", "SDBPacket created");
    }

    public SDBPacket(CmdType cmd, Byte... param)
    {
        this.cmd = cmd;
        parameters = new ArrayList<Byte>();
        for (byte b : param) {
            parameters.add(b);
        }
        Log.d("[ABS]", "SDBPacket created");
    }

    public byte[] toRaw()
    {
        byte[] byteArray = new byte[parameters.size()];
        for(int i = 0; i < parameters.size(); i++) {
            byteArray[i] = parameters.get(i).byteValue();
        }
        return concatenate(cmd.getCmd(), byteArray);
    }

    private byte[] concatenate(byte[]...arrays)
    {
        int totalLength = 0;
        for (byte[] array : arrays) {
            totalLength += array.length;
        }
        byte[] result = new byte[totalLength];
        int currentIndex = 0;
        for (byte[] array : arrays) {
            System.arraycopy(array, 0, result, currentIndex, array.length);
            currentIndex += array.length;
        }
        return result;
    }

    public void addParameter(byte param)
    {
        parameters.add(param);
        Log.d("[ABS]", "SDB parameter added");
    }

    public byte getParameter(int pos)
    {
        return parameters.get(pos);
    }
}