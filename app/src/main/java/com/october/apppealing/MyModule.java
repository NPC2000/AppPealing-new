package com.october.apppealing;

import android.content.Context;

import java.io.File;
import java.lang.reflect.Method;
import java.util.function.Predicate;
import java.util.function.Supplier;

import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class MyModule implements IXposedHookLoadPackage {

    public static final String TAG = "AppSealing";

    public void patchMethod(XC_LoadPackage.LoadPackageParam param,
                            String className, Predicate<Method> methodPredicate, Supplier<Object> returnValue) {
       Class<?> clazz;
        try {
            clazz = XposedHelpers.findClass(className, param.classLoader);
        } catch (XposedHelpers.ClassNotFoundError ex) {
            XposedBridge.log(TAG +": " + param.packageName + " cannot find class " + className);
            return;
        }

        for (Method method : clazz.getDeclaredMethods()) {
            if (!methodPredicate.test(method))
                continue;
            XposedBridge.hookMethod(method, new XC_MethodHook() {
                @Override
                protected void beforeHookedMethod(MethodHookParam methodParam) throws Throwable {
                    XposedBridge.log(String.format("%s: redirected call to %s#%s", TAG, className, method.getName()));
                    methodParam.setResult(returnValue.get());
                }
            });
            XposedBridge.log(String.format("%s: hooked %s#%s", TAG, className, method.getName()));
        }
    }

    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam param) throws Throwable {
        XposedBridge.log(TAG +": loaded app " + param.packageName);
        try {
            XposedHelpers.findClass("com.inka.appsealing.AppSealingApplication", param.classLoader);
        } catch (XposedHelpers.ClassNotFoundError ex) {
            XposedBridge.log(TAG +": " + param.packageName + " doesn't seem to have AppSealing, skipped.");
            return;
        }

        // Patch out telemetry
        patchMethod(param, "com.inka.appsealing.AwsSqsSender", m -> "send".equals(m.getName()), () -> true);
        // Patch out kill
        patchMethod(param, "com.inka.appsealing.AppSealingAlertDialog", m -> "killMyProcess".equals(m.getName()), () -> null);
        patchMethod(param, "com.inka.appsealing.AppSealingAlertDialog", m -> "showAlertDialog".equals(m.getName()), () -> null);
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
