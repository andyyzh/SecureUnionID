/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */
package com.volcengine.secureunionid;

public class SecureUnionIDJNI {
  public final static native void HASHIT(String jarg1, String jarg2);
  public final static native long randomSeed();
  public final static native int MasterKeygen(long jarg1, byte[] jarg2);
  public final static native int Keygen(byte[] jarg1, String jarg2, byte[] jarg3, byte[] jarg4, byte[] jarg5);
  public final static native int System_Keygen(String[] jarg1, String[] jarg2, int jarg3, byte[] jarg4, byte[] jarg5);
  public final static native int Blinding(String jarg1, long jarg2, byte[] jarg3, byte[] jarg4);
  public final static native int Enc(byte[] jarg1, byte[] jarg2, byte[] jarg3);
  public final static native int Unblinding(String[] jarg1, int jarg2, byte[] jarg3, byte[] jarg4, byte[] jarg5);
  public final static native int verify_individual(String[] jarg1, String[] jarg2, String[] jarg3, String jarg4, int jarg5, byte[] jarg6);
  public final static native int batch_verify(String[] jarg1, String[] jarg2, byte[] jarg3, int jarg4);
}
