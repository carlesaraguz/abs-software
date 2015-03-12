package com.abs.sdb;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class SDB {

    private Socket socket;
    private BufferedReader rxStream;
    private PrintWriter txStream;

    /**
     *  SDB constructor
     */

    public void SDB()
    {
        try {
            socket = new Socket("127.0.0.1", 1111);
            rxStream = new BufferedReader(
                    new InputStreamReader(socket.getInputStream()));
            txStream = new PrintWriter(socket.getOutputStream());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Send a command to the SDB
     *
     * @param packet    the packet that will be send to the SDB
     * @return          the response of the SDB to the packet
     */

    public synchronized byte[] send(SDBPacket packet)
    {
        txStream.println(packet.toRaw());
        try {
            String res = rxStream.readLine();
            return res.getBytes();
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }
}