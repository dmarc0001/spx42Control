/*
  erzeuge Startmenueintrag, Desktop Icon
*/

function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") 
    {    
        component.addOperation("CreateShortcut", "@TargetDir@/spx42Control.exe", "@StartMenuDir@/SPX42 Control.lnk", "workingDirectory=@TargetDir@", "iconId=1");
        component.addOperation("CreateShortcut", "@TargetDir@/spx42Control.exe", "@DesktopDir@/SPX42 Control.lnk", "workingDirectory=@TargetDir@", "iconId=1");
        component.addOperation("CreateShortcut", "@TargetDir@/maintenancetool.exe", "@StartMenuDir@/SUBMATIXUpdater.lnk", "workingDirectory=@TargetDir@", "iconId=1");
    }
}