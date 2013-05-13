function OnCreate(self)
	self:Center()
end
function OnCancelClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(0)
end

function OnNewTaskItemCheck(self, eventName, index, isCheck)
	--local owner = self:GetOwner()
	--local spaceNeeded = owner:GetUIObject("taskspace.content")
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	--XLMessageBox( self:GetItemByIndex(index + 1):GetControlObject("name"):GetText() )
	local userData = hostwnd:GetUserData()
	if userData.SelectedFiles == nil then
		userData.SelectedFiles = {}
	end
	table.insert(userData.SelectedFiles, self:GetItemByIndex(index + 1):GetControlObject("name"):GetText())
end

function OnOKClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local userData = hostwnd:GetUserData()
	local savepath = owner:GetUIObject("savepath"):GetText()
	local torrentpath = userData.path
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	local ret = coolClientProxy:AddNewDownload(torrentpath, savepath, userData.SelectedFiles)
	if ret == -1 then
		XLMessageBox("error")
	end
end