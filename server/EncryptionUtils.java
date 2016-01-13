import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.KeyFactory;
import java.security.NoSuchAlgorithmException;
import java.security.PublicKey;
import java.security.interfaces.RSAPublicKey;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.RSAPublicKeySpec;

import javax.crypto.Cipher;
import javax.crypto.SecretKey;


public class EncryptionUtils {
	private SecretKey AESKey;
	private byte[] ClientRSAkey;
	
	private static final boolean IS_USING_ENCRYPTION = true;
	
	
	public EncryptionUtils(SecretKey AESKey, byte[] ClientRSAkey) {
		this.AESKey = AESKey;
		this.ClientRSAkey = ClientRSAkey;
		
	}
	
	//TODO: change the randomly generated server key to a byte array of keysize
	public byte[] getServerKey() throws InvalidKeySpecException, NoSuchAlgorithmException {
		return encryptByClientPublicKey(AESKey.getEncoded());
	}
	
	//encrypt messages by AESKeyw
		public byte[] encryptByClientAESKey(byte[] message) {
			
			if (!IS_USING_ENCRYPTION)
				return message;
			
//			System.out.println("start encrypting by aes key");
//			System.out.println("Before Encryption: ");
		    byte[] cipherText = null;
		    try {
			      Cipher cipher = Cipher.getInstance("AES");
			      cipher.init(Cipher.ENCRYPT_MODE, AESKey);
			      cipherText = cipher.doFinal(message);
			    } catch (Exception ex) {
			      ex.printStackTrace();
			    }
		    return cipherText;
		}
		
		
		// TODO: should only require a byte array. The keys should be store in class
		// and not required as a parameter
		public byte[] decryptByClientAESKey(byte[] message) {
			if (!IS_USING_ENCRYPTION)
				return message;
			
			
			
			//System.out.println("start decrypting by aes key");
			
		    byte[] dectyptedText = null;
		    
//		    System.out.println("AES Key length"+AESKey.getEncoded().length);
		    
		    try {
		    	
		      Cipher cipher = Cipher.getInstance("AES");
		      cipher.init(Cipher.DECRYPT_MODE, AESKey);
		      dectyptedText = cipher.doFinal(message);
		      
		    } catch (Exception ex) {
		      ex.printStackTrace();
		    }
		    return dectyptedText;
	        
		  }
	
	private byte[] encryptByClientPublicKey(byte[] message) throws InvalidKeySpecException, NoSuchAlgorithmException {
		PublicKey publicKey = encodedRSAKey();
		//System.out.println("public key length: " + ((RSAPublicKey)publicKey).getModulus().bitLength());
	    byte[] cipherText = null;
	    
	    try {
	    	
	      Cipher cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
	      cipher.init(Cipher.ENCRYPT_MODE, publicKey);
	      //System.out.println("cipher block size: "+ cipher.getBlockSize());
	      cipherText = cipher.doFinal(message);
	      
	    } catch (Exception e) {
	      e.printStackTrace();
	    }
	    return cipherText;
	}
	
	private PublicKey encodedRSAKey() throws InvalidKeySpecException, NoSuchAlgorithmException{
		ByteBuffer buffer = ByteBuffer.wrap(ClientRSAkey).order(ByteOrder.LITTLE_ENDIAN);
		
		int length1 = buffer.getInt();
		//System.out.println("length 1" + length1);
		byte[] modulus = new byte[length1];
		buffer.get(modulus, 0, length1);
		
		BigInteger modulusBigInteger = new BigInteger(1, modulus);
		
		int length2 = buffer.getInt();
		byte[] publicExponent = new byte[length2];
		//System.out.println("length2 :" + length2);
		buffer.get(publicExponent, 0, length2);
		
		BigInteger publicExponentBigInteger = new BigInteger(1, publicExponent);
		
		//System.out.println("BigInteger E: " + publicExponentBigInteger);
		

		PublicKey publicKey = KeyFactory.getInstance("RSA").generatePublic(new RSAPublicKeySpec(modulusBigInteger, publicExponentBigInteger));
		return publicKey;
	}
	

	
	
}
