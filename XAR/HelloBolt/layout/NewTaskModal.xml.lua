function OnCreate(self)
	self:Center()
end
function OnCancelClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(0)
end

function OnNewTaskItemCheck(self, eventName, index, isCheck)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local spaceNeeded = owner:GetUIObject("taskspace.content")
	local listbox = owner:GetUIObject("listbox")
	local listboxAttr = listbox:GetAttribute()
	--XLMessageBox( self:GetItemByIndex(index + 1):GetControlObject("name"):GetText() )
	local userData = hostwnd:GetUserData()
	if userData.TotalSize == nil then
		userData.TotalSize = 0
	end
	if userData.SelectedFiles == nil then
		userData.SelectedFiles = {}
	end
	if isCheck then
		table.insert(userData.SelectedFiles, index, self:GetItemByIndex(index):GetControlObject("name"):GetText())
	else
		table.remove(userData.SelectedFiles, index)
	end
	if isCheck then
		userData.TotalSize = userData.TotalSize + listboxAttr.ItemDataTable[index].Size
	else
		userData.TotalSize = userData.TotalSize - listboxAttr.ItemDataTable[index].Size
	end
	if spaceNeeded ~= nil then
		local KB = 1024
		local MB = KB*1024
		local GB = MB*1024
		local content
		if userData.TotalSize > GB then
			content = string.format("%.2f GB", userData.TotalSize/GB)
		elseif userData.TotalSize > MB then
			content = string.format("%.2f MB", userData.TotalSize/MB)
		elseif userData.TotalSize > KB then
			content = string.format("%.2f KB", userData.TotalSize/KB)
		else
			content = string.format("%d B", userData.TotalSize)
		end
	    spaceNeeded:SetText(content)
	end
end

function OnOKClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local userData = hostwnd:GetUserData()
	local savepath = owner:GetUIObject("savepath"):GetText()
	local torrentpath = userData.path
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	--for i, v in ipairs(userData.SelectedFiles) do
	--	XLMessageBox(i .. ' -> ' .. v )
	--end
	local ret = coolClientProxy:AddNewDownload(torrentpath, savepath, userData.SelectedFiles)
	if ret == -1 then
		XLMessageBox("error")
	end
end