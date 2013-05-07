function OnOKClick(self)	
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	
	local treeManager = XLGetObject("Xunlei.UIEngine.TreeManager")				
	local tree = treeManager:GetUIObjectTree("MainObjectTree")
	local configPage = tree:GetUIObject("tabbkg"):GetControlObject("ConfigPage")
	configPage:ConfigReset()

	hostwnd:EndDialog(0)
end

function OnCancelClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(0)
end