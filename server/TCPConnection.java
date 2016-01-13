import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;


public class TCPConnection implements Runnable {
	//Thread t = new Thread(new ConnectionHandler(s1));
	//t.start();
	
	private Socket serverSocket;
	private byte[] message;
	private EncryptionUtils encrypt_decrypt;
	private int connectionId;
	private OutputStream clientOutputStream;
	
	private static final int MAGICNUMBER_SERVERTOCLIENT = 0xF822A294;

	
	private static final byte SEND_DATA_BYTE = 3;
	
	public TCPConnection(InetAddress addr, int port, byte[] message, EncryptionUtils encryptionUtils, int connectionId, OutputStream clientOutputStream) {
		try {
			this.serverSocket = new Socket(addr, port);
			this.message = message;
			this.encrypt_decrypt = encryptionUtils;
			this.connectionId = connectionId;
			this.clientOutputStream = clientOutputStream;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	//public void run(){}
	
	@Override
	public void run() {
		try {
			DataInputStream serverResponseOnServer = new DataInputStream(new BufferedInputStream(serverSocket.getInputStream()));
			DataOutputStream clientRequestOnServer = new DataOutputStream(new BufferedOutputStream(serverSocket.getOutputStream()));

			System.out.println("start of message");
	        System.out.println(new String(message, "UTF-8").substring(0, Math.min(message.length, 30)));
	        System.out.println("end of message");

			System.out.println("Time for starting request on the server is " + System.currentTimeMillis());

			
			clientRequestOnServer.write(message); // write the decrypted message from client to the server
			clientRequestOnServer.flush();
			
			

			byte[] serverResponse = new byte[0];
			
			byte[] buf = new byte[4096];					    
			int read = 0;
			
	        while ((read = serverResponseOnServer.read(buf)) != -1) {
	        	System.out.println("read " + read + " bytes");
		        //System.out.println(new String(buf, "UTF-8"));

	        	byte[] tempServerResponse = new byte[serverResponse.length + read];
	        	System.arraycopy(serverResponse, 0, tempServerResponse, 0, serverResponse.length);
	        	System.arraycopy(buf, 0, tempServerResponse, serverResponse.length, read);
	        	serverResponse = tempServerResponse;
	        	if (read < 4096)
	        		break;
	        }
	        
	    
	        String serverResponseString = new String(serverResponse, "UTF-8");
	        System.out.println("serverResponse " + serverResponseString.substring(0, Math.min(30, serverResponseString.length())));

			System.out.println("Time for end getting message from the server is " + System.currentTimeMillis());


	        int preEncryptedLength = 1 + 4 + serverResponse.length;
	        
	        ByteBuffer unencryptedBuffer = ByteBuffer.allocate(preEncryptedLength).order(ByteOrder.LITTLE_ENDIAN);
	        
	        unencryptedBuffer.put(SEND_DATA_BYTE);
	        unencryptedBuffer.putInt(connectionId);
	        unencryptedBuffer.put(serverResponse);
	        
	        byte[] encryptedResponse = encrypt_decrypt.encryptByClientAESKey(unencryptedBuffer.array());

	        int length = 4 + 4 + encryptedResponse.length; // length + magic number + encrypted part
	        
	        
	        
	        
	        
	        ByteBuffer responseBuffer = ByteBuffer.allocate(length).order(ByteOrder.LITTLE_ENDIAN);
	        responseBuffer.putInt(length);
	        responseBuffer.putInt(MAGICNUMBER_SERVERTOCLIENT);
	        
	        responseBuffer.put(encryptedResponse);
	        
	        byte[] wrappedResponseFromServer = responseBuffer.array();
	        
	        System.out.println("the encrypted message length " + length);
	        
	        
	        System.out.println("The original message bytes are " + serverResponse.length + " bits");
	        
	        for (int i = 0; i < Math.min(30, serverResponse.length); i++)
	        	System.out.printf("%02X ", serverResponse[i]);
	        
	        System.out.println("\nThe encrytpted message bytes are");
	        
	        for (int i = 0; i < Math.min(30, encryptedResponse.length); i++)
	        	System.out.printf("%02X ", encryptedResponse[i]);
	        
	        System.out.println("\nwrapped response");
	        for (int i = 0; i < Math.min(30, wrappedResponseFromServer.length); i++)
	        	System.out.printf("%02X ", wrappedResponseFromServer[i]);
	        
	        clientOutputStream.write(wrappedResponseFromServer);
	        clientOutputStream.flush();	
	        
			System.out.println("Time for end writting message to the client is " + System.currentTimeMillis());

		} catch (Exception e) {
			throw new IllegalStateException(e);
		}
	
	}}
