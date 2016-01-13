import java.io.IOException;
import java.security.NoSuchAlgorithmException;
import java.security.spec.InvalidKeySpecException;

import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;


public class VPNServerMain {

	public static void main(String[] args) throws InvalidKeySpecException, NoSuchAlgorithmException, IOException {
		// TODO Auto-generated method stub
		VPNServer vpnserver = new VPNServer();
		vpnserver.startVPNServer();
		
//		byte[] message1 = new byte[]{(byte) 244, (byte)206, 108, (byte)190, (byte)191, 71, 86, 124, 42, 25, 41, (byte)144, 65, (byte)184,(byte) 200, (byte)164};
//		byte[] message = new byte[]{(byte)1,(byte)12, (byte)0, (byte)0, (byte)0, (byte)-128, (byte)-48, (byte)3, (byte)88, (byte)0, (byte)80, (byte)5, (byte)5, (byte)5, (byte)5, (byte)5};
//		byte[] key = new byte[]{0x01,(byte) 0xdd, 0x32, (byte) 0x95, 0x25, 0x74, 0x3b, 0x4a, (byte) 0xf2, (byte) 0xae, (byte) 0xac, (byte) 0xaa, 0x1c, (byte) 0xa9, 0xc, 0x39};
//		
//		SecretKeySpec secretKeySpec = new SecretKeySpec(key, "AES"); 
//		
//		
//
//		
//		System.out.println("start encrypting by aes key");
//		System.out.println("message length " + message.length);
//	    byte[] dectyptedText = null;
////	    System.out.println("AES Key length"+AESKey.getEncoded().length);
//	    try {
//	      Cipher cipher = Cipher.getInstance("AES/ECB/NoPadding");
//	      cipher.init(Cipher.ENCRYPT_MODE, secretKeySpec);
//	      dectyptedText = cipher.doFinal(message);
//	    } catch (Exception ex) {
//	      ex.printStackTrace();
//	    }
////	    String decrpyt = new String(dectyptedText);
////	    byte[] de = decrpyt.getBytes();
//	    for(int i = 0; i < dectyptedText.length; i++){
//	    	if(dectyptedText[i] != message1[i]){
//	    		System.out.println("error");
//	    	}
//	    	System.out.print(dectyptedText[i] + " ");
//	    	
//	    }
//	    System.out.println(decrpyt);
//	    return decrpyt.getBytes();
	}

}
