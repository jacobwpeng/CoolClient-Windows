function OnPageInit(self)
	self:SetObjPos(0,0,"father.width","father.height")
	--然后还要载入保存的任务列表
	
	local list = self:GetControlObject("listbox")
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	coolClientProxy:RunClientAsync()
	self:GetAttribute().TaskShowType = 'all'
	local timer = SetTimer(function() self:UpdateListBox() end, 1000)
end

function OnCronClick(self)
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")				
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local parent = hostwndManager:GetHostWnd("MainFrame")
	local tab = parent:GetUserData()
	local parentHandle = parent:GetWndHandle()
	
	local menuTreeTemplate = templateMananger:GetTemplate("MydownloadPage.CronMenu","ObjectTreeTemplate")
	local menuTree = menuTreeTemplate:CreateInstance("Thunder.CronMenu.Instance")
	local menuHostWndTemplate = templateMananger:GetTemplate("Thunder.MainMenuHostwnd", "HostWndTemplate")
	local menuHostWnd = menuHostWndTemplate:CreateInstance("Thunder.MenuHostWnd.Instance")
	

	menuHostWnd:BindUIObjectTree(menuTree)
	local menuContext = menuTree:GetUIObject("root")
	if tab.Cron ~= nil then
		menuTree:GetUIObject(tab.Cron):SetText("取消")
	end
	--获取父窗口的宽度还未实现，更无法实现窗口可变
	local left,top,right,bottom = self:GetOwnerControl():GetControlObject("bottombkg"):GetObjPos()
	local x,y = parent:ClientPtToScreenPt(left,top)
	menuHostWnd:TrackPopupMenu(parentHandle, x + 60, y, x + 150, y + 66)
	
	local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")	
	objtreeManager:DestroyTree("Thunder.CronMenu.Instance")
	hostwndManager:RemoveHostWnd("Thunder.MenuHostWnd.Instance")
	
	--XLMessageBox(self:GetOwner():GetID())
end

function OnCronMenuItemClick(self)
	local id = self:GetID()
	local text = self:GetControlObject("text")
	local treeManager = XLGetObject("Xunlei.UIEngine.TreeManager")				
	local tree = treeManager:GetUIObjectTree("MainObjectTree")
	local cronBtn = tree:GetUIObject("tabbkg"):GetControlObject("MydownloadPage"):GetControlObject("cron")
	
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local host = hostwndManager:GetHostWnd("MainFrame")
	local tab = host:GetUserData()
	
	if tab.Cron == nil then
		if id == "item.shutdown" then
			--下载后关机
		elseif id == "item.exit" then
			--下载后退出
		elseif id == "item.stopupload" then
			--下载后停止上传
		end
		tab.Cron = id
		cronBtn:SetText(text:GetText())
	else
		tab.Cron = nil
		cronBtn:SetText("计划任务")
	end
end

--listbox选中发生变化，这里用来更新按钮之类的 index从1开始
function OnListBoxSelectChanged(self,eventname,index)
	--XLMessageBox(self:GetItemByIndex(index):GetClass())
	local attr = self:GetAttribute()
	local page = self:GetOwnerControl()
	local panel = page:GetControlObject("controlPanel")
	
	local start = panel:GetControlObject("btn.start")
	local pause = panel:GetControlObject("btn.pause")
	local delete = panel:GetControlObject("btn.delete")
	local openfolder = panel:GetControlObject("btn.openfolder")
	
	delete:SetEnable(true)
	openfolder:SetEnable(true)
	--
	if attr.ItemDataTable[index].Status == 1 then
		start:SetEnable(true)
		pause:SetEnable(false)
	elseif attr.ItemDataTable[index].Status == 2 then
		start:SetEnable(false)
		pause:SetEnable(true)
	elseif attr.ItemDataTable[index].Status == 3 then
		start:SetEnable(false)
		pause:SetEnable(false)
	elseif attr.ItemDataTable[index].Status == 4 then
		start:SetEnable(false)
		pause:SetEnable(false)
	end
end

function UpdateListBox(self)
	local listbox = self:GetControlObject("listbox")
	local statusbar = self:GetControlObject('tasklist.header')	
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	local jobTable = coolClientProxy:GetJobStatusTable()
	
	if statusbar then
		local attr = statusbar:GetAttribute()
		if attr.SortBy ~= nil and jobTable ~= -1 then
			statusbar:Sort(jobTable)
		end
	end
	local downloadSpeed = 0--self:GetControlObject("text.download")
	local uploadSpeed = 0--self:GetControlObject("text.upload")
	local listboxAttr = listbox:GetAttribute()

	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	local jobTable = coolClientProxy:GetJobStatusTable()
	local attr = self:GetAttribute()
	if jobTable ~= -1 then
		--XLMessageBox(#jobTable)
		local curSelIndex = listbox:GetCurSel()
		listbox:ResetContent()
		if attr.TaskShowType == 'all' then
			for k,v in pairs(jobTable) do
				listbox:AddItem(v)
				downloadSpeed = downloadSpeed + v.DownloadSpeed
				uploadSpeed = uploadSpeed + v.UploadSpeed
			end
		elseif attr.TaskShowType == 'finished' then
			for k,v in pairs(jobTable) do
				if v.Progress == 100 then
					listbox:AddItem(v)
				end
				downloadSpeed = downloadSpeed + v.DownloadSpeed
				uploadSpeed = uploadSpeed + v.UploadSpeed
			end
		elseif attr.TaskShowType == 'downloading' then
			for k,v in pairs(jobTable) do
				if v.Status == 2 then
					listbox:AddItem(v)
				end
				downloadSpeed = downloadSpeed + v.DownloadSpeed
				uploadSpeed = uploadSpeed + v.UploadSpeed
			end
		elseif attr.TaskShowType == 'active' then
			for k,v in pairs(jobTable) do
				if v.Status == 2 or v.Status == 3 then
					listbox:AddItem(v)
				end
				downloadSpeed = downloadSpeed + v.DownloadSpeed
				uploadSpeed = uploadSpeed + v.UploadSpeed
			end
		elseif attr.TaskShowType == 'stopped' then
			for k,v in pairs(jobTable) do
				if v.Status == 0 then
					listbox:AddItem(v)
				end
				downloadSpeed = downloadSpeed + v.DownloadSpeed
				uploadSpeed = uploadSpeed + v.UploadSpeed
			end
		end
		--XLMessageBox(curSelIndex)
		listbox:UpdateUI()
		local KB = 1024
		local MB = KB*1024
		local content
		if downloadSpeed > MB then
			content = string.format("%.1fMB/s", downloadSpeed/MB)
		elseif downloadSpeed > KB then
			content = string.format("%dKB/s", downloadSpeed/KB)
		else
			content = string.format("%dB/s", downloadSpeed)
		end
		self:GetControlObject("text.download"):GetChildByIndex(0):SetText(content)
		if uploadSpeed > MB then
			content = string.format("%.1fMB/s", uploadSpeed/MB)
		elseif uploadSpeed > KB then
			content = string.format("%dKB/s", uploadSpeed/KB)
		else
			content = string.format("%dB/s", uploadSpeed)
		end
		self:GetControlObject("text.upload"):GetChildByIndex(0):SetText(content)
		--XLMessageBox(content)
	end
	
end

function AddNewDownloadTask(self, path, name, torrent_type, files)
	if path == -1 then
		--不是种子文件
		return 
	else
		--XLMessageBox(path)
		--XLMessageBox(name)
		--XLMessageBox(string.format("path:%s name:%s type:%s"),type(path),type(name),type(torrent_type))
		local templateManager = XLGetObject("Xunlei.UIEngine.TemplateManager")
		local hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
		local mainWnd = hostWndManager:GetHostWnd("MainFrame")
		local modalHostWndTemplate = templateManager:GetTemplate("Thunder.NewTaskModal","HostWndTemplate")
		local modalHostWnd = modalHostWndTemplate:CreateInstance("Thunder.NewTaskModal.Instance")
		local objectTreeTemplate = templateManager:GetTemplate("Thunder.NewTaskModal","ObjectTreeTemplate")
		local uiObjectTree = objectTreeTemplate:CreateInstance("Thunder.NewTaskModal.Instance")
		local folderName = uiObjectTree:GetUIObject("foldername")
		local typeobj = uiObjectTree:GetUIObject("type")
		local listbox = uiObjectTree:GetUIObject("listbox")
		if torrent_type and typeobj then
			if torrent_type == 1 then
				typeobj:SetResID("bitmap.listbox.taskitem.type.movie")
			elseif torrent_type == 2 then
				typeobj:SetResID("bitmap.listbox.taskitem.type.music")
			elseif torrent_type == 4 then
				typeobj:SetResID("bitmap.listbox.taskitem.type.game")
			elseif torrent_type == 8 then
				typeobj:SetResID("bitmap.listbox.taskitem.type.book")
			end
		end
		for key, value in pairs(files) do
			listbox:AddItem({Name = key, Size =  value})
		end
		listbox:UpdateUI()

		folderName:SetText(name)
		modalHostWnd:BindUIObjectTree(uiObjectTree)
		local userData = {path = path, name = name, torrent_type = torrent_type, files = files}
		modalHostWnd:SetUserData(userData)
		modalHostWnd:DoModal(mainWnd:GetWndHandle())

		local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")	
		objtreeManager:DestroyTree("Thunder.NewTaskModal.Instance")
		hostWndManager:RemoveHostWnd("Thunder.NewTaskModal.Instance")
	end
end