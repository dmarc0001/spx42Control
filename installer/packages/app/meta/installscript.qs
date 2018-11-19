/*
  erzeuge Startmenueintrag
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
        component.addOperation("CreateShortcut", "@TargetDir@/spx42Control.exe", "@StartMenuDir@/SPX42 Control.lnk",
            "workingDirectory=@TargetDir@", "@TargetDir@/spx42Control.exe","iconId=1");
        component.addOperation("CreateShortcut", "@TargetDir@/maintenancetool.exe", "@StartMenuDir@/Maintenance Tool Updater.lnk",
            "workingDirectory=@TargetDir@", "@TargetDir@/maintenancetool.exe","iconId=1");
    }
}