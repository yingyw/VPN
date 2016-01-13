import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.NoSuchAlgorithmException;
import java.security.PublicKey;
import java.security.SecureRandom;
import java.security.spec.InvalidKeySpecException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicInteger;

import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;

public class VPNServer {

	public static final String GENERATOR_ALGORITHM = "RSA";
	public static final int KEY_SIZE = 1024;
	public static final int port = 8080;
	

	private ConcurrentLinkedQueue<Integer> connectionQueue;
	private ConcurrentHashMap<Integer, Integer> validConnection;

	
	// AtomicInteger and ConcurrentMap type are used to deal with concurrency issues
	private AtomicInteger currentClientId;
	private SecretKey AESKey;
	
	public VPNServer() {
		
		try {
			currentClientId = new AtomicInteger(0);
			connectionQueue = new ConcurrentLinkedQueue<Integer>();
			validConnection = new ConcurrentHashMap<Integer, Integer>();
			
			
			KeyGenerator keygen = KeyGenerator.getInstance("AES"); 
			keygen.init(128);
	        SecureRandom random = new SecureRandom();  
	        keygen.init(random);  
	        this.AESKey = keygen.generateKey(); 
	        byte[] check = AESKey.getEncoded();
	        
	        System.out.println("The VPN server has started");
//	        System.out.print("AESKey : ");
//	        for(int i = 0; i < AESKey.getEncoded().length; i++){
//	        	
//	        	System.out.print(String.format("%02X ", check[i]));
//	        }
//	        System.out.println("key size: " + AESKey.getEncoded().length);
			
		} catch (NoSuchAlgorithmException e) {
			System.err.println("Cannot generate public and private key");
			System.exit(1);
		}
	}
	
	public void startVPNServer() throws IOException, InvalidKeySpecException, NoSuchAlgorithmException {
		@SuppressWarnings("resource")
		ServerSocket serverSoc = new ServerSocket(port);
		while (true) {
			Socket socket = serverSoc.accept();
			Thread t = new Thread(new VPNConnection(socket, connectionQueue, validConnection, AESKey, currentClientId));
    		t.start();
		}
		
	}
	
	
	

}
