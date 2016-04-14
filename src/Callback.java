import com.dashoptimization.*;

class Callback implements XPRSmessageListener
{
    public void run ()
    {
        try {
            XPRSprob problem = new XPRSprob ();

            problem.setIntControl (XPRS.LPLOG, 1);
            problem.addMessageListener(this,null);

            problem.readProb ("..\\data\\javatest","");
            problem.minim ("");

            problem.removeMessageListener();
        } catch(XPRSprobException xpe) {
            xpe.printStackTrace();
        }
    }

    public void XPRSmessageEvent(XPRSprob prob,
        Object data, String msg, int len, int type)
    {
        System.out.println (msg);
    }

    public static void main(String [] args)
    {
        try {
            XPRS.init ();
        } catch (Exception e) {
            System.out.println ("Failed to initialize");
            return;
        }

        try {
          Callback c = new Callback ();
          c.run ();

        } finally {
          XPRS.free ();
        }
    }
}
