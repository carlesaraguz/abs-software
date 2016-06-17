/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package ring.based.v3;

import static java.lang.Thread.sleep;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;


/**
 *
 * @author Sebas
 */
public class ringBasedV3Test {
    
    public ringBasedV3Test() {
    }
    
    @BeforeClass
    public static void setUpClass() {
    }
    
    @AfterClass
    public static void tearDownClass() {
    }
    
    @Before
    public void setUp() {
    }
    
    @After
    public void tearDown() {
    }

    /**
     * Test of main method, of class ringBasedV3.
     * @throws java.lang.InterruptedException
     */
    @Test
    public void testMain() throws InterruptedException {
        int nsatmin = 1;
        int nsatmax = 10;
        int nsatStep = 1;
        
        int ntaskmin = 1;
        int ntaskmax = 10;      
        int ntaskStep = 1;
        
        int numrep = 1;
        
        String[] args = new String[2];        
        for(int i = nsatmin; i<= nsatmax; i = i+nsatStep){
            for(int j = ntaskmin; j<= ntaskmax; j = j+ntaskStep){
                for(int k=1; k<=numrep; k++){
                    if(i==0){
                        args[0] = Integer.toString(1);
                    } else {
                        args[0] = Integer.toString(i);
                    }
                    if(j==0){
                        args[1] = Integer.toString(1);
                    } else {
                        args[1] = Integer.toString(j);
                    }                   
                    ringBasedV3.main(args);  
                    sleep(2000);
                }
            }
        }        
    }
    
}
