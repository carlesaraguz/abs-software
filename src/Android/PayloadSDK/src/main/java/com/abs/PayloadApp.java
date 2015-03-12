package com.abs;

import com.abs.sdb.SDB;

public class PayloadApp {

    private SDB sdb;
    private Arduino arduino;

    /**
     * PayloadApp constructor
     */

    public PayloadApp()
    {
        sdb = new SDB();
    }

    /**
     *
     * @return
     */

    public Arduino getArduino()
    {
        return (arduino = new Arduino(sdb));
    }


}




