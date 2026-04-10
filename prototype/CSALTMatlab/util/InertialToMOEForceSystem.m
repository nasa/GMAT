function R = InertialToMOEForceSystem(moeElements,gravParam)
   cartState = Mee2Cart(moeElements,gravParam);
   rVec = cartState(1:3,1);
   vVec = cartState(4:6,1);
   hVec = cross(rVec,vVec);
   rVecU = rVec/norm(rVec);
   hVecU = hVec/norm(hVec);
   R = [rVecU cross(hVecU,rVecU) hVecU];
   R = R';
end