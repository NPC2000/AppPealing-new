package com.october.apppealing;

import android.content.Context;

import java.io.File;

import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class MyModule implements IXposedHookLoadPackage {

    public static final String TAG = "AppSealing";

    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam param) throws Throwable {
        XposedBridge.log(TAG +": loaded app " + param.packageName);

        // Patch out telemetry
        XposedHelpers.findAndHookMethod("com.inka.appsealing.AwsSqsSender", param.classLoader, "send", String.class, String.class, String.class, String.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                XposedBridge.log(TAG + ": redirected call to AwsSqsSender#send");
                param.setResult(true);
            }
        });

        // Patch out kill
        XposedHelpers.findAndHookMethod("com.inka.appsealing.AppSealingAlertDialog", param.classLoader, "killMyProcess", int.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                XposedBridge.log(TAG + ": redirected call to AppSealingAlertDialog#killMyProcess");
                param.setResult(null);
            }
        });
        XposedHelpers.findAndHookMethod("com.inka.appsealing.AppSealingAlertDialog", param.classLoader, "showAlertDialog", Context.class, int.class, Object.class, File.class, boolean.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                XposedBridge.log(TAG + ": redirected call to AppSealingAlertDialog#showAlertDialog");
                param.setResult(null);
            }
        });

        // Load patch library
        XposedHelpers.findAndHookMethod("com.inka.appsealing.AppSealingApplication", param.classLoader, "attachBaseContext", Context.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                XposedBridge.log(TAG + ": Loading libpeal");
                System.loadLibrary("apppealing");
                XposedBridge.log(TAG + ": Loaded libpeal");
            }
        });
    }

}
