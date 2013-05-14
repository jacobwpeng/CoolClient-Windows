--这里还是用了很多静态的值，可以做修改，修改后如果素材发生了变化只用修改XML文件
local function deepcopy(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end  -- if
        local new_table = {}
        lookup_table[object] = new_table
        for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)
        end  -- for
        return setmetatable(new_table, getmetatable(object))
    end  -- function _copy
    return _copy(object)
end  -- function deepcopy

function AddBtn(self, userdata)
	local panelattr = self:GetAttribute()
	local fillObj = self:GetControlObject("ctrl")
	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	local newBtn = objFactory:CreateUIObject(BtnName,"Thunder.ControlPanel.Btn")
	local btnattr = newBtn:GetAttribute()
	
	btnattr.BtnName = userdata.BtnName
	btnattr.NormalResID = userdata.NormalResID
	btnattr.DisabledResID = userdata.DisabledResID
	btnattr.BtnStatus = userdata.BtnStatus
	btnattr.AniImagelistID = userdata.AniImagelistID
	--XLMessageBox(userdata.AniImagelistID)
	local left, top, right, bottom = fillObj:GetObjPos()
	local width = right - left
	local preChildrenWidth = 0
	if panelattr.BtnNum ~= 0 then
		preChildrenWidth = panelattr.BtnNum*38 + ( panelattr.BtnNum-1 )*3
	end
	
	if panelattr.BtnNum ~= 0 then
		local splitter = objFactory:CreateUIObject("ControlPanelSplitter"..panelattr.BtnNum,"Thunder.ControlPanel.Separate")
		local splitterimg = splitter:GetControlObject("Thunder.ControlPanel.Separate.Image")
		splitterimg:SetResID("toolbar.separate")
		splitter:SetObjPos(preChildrenWidth, 0, width-preChildrenWidth, 0)
		fillObj:AddChild(splitter)
		newBtn:SetObjPos(preChildrenWidth + 3, 0, width - 3 - preChildrenWidth, 0)
	else 
		newBtn:SetObjPos(preChildrenWidth, 0, width - preChildrenWidth, 0)
	end
	fillObj:AddChild(newBtn)
	
	panelattr.BtnNum = panelattr.BtnNum + 1
end

function OnBtnInit(self)
	local attr = self:GetAttribute()
	local ctrlobj = self:GetControlObject("ctrl")
	local imgobj = self:GetControlObject("btn.img")
	local left1, top1, right1, bottom1 = imgobj:GetObjPos()
	local left2, top2, right2, bottom2 = ctrlobj:GetObjPos()
	local ctrlwidth = right2-left2
	local ctrlheight = top2-bottom2
	local imgobjwidth = right1-left1
	local imgobjheight = top1-bottom1
	--XLMessageBox(ctrlwidth.." "..imgobjwidth)
	--imgobj:SetObjPos( (ctrlwidth-imgobjwidth)/2, (ctrlheight-imgobjheight)/2, ctrlheight-(ctrlwidth-imgobjwidth)/2, ctrlheight-(ctrlheight-imgobjheight)/2)
	--XLMessageBox(self)
	if attr.BtnStatus=="normal" then
		imgobj:SetResID(attr.NormalResID)
		self:SetEnable(true)
	else
		imgobj:SetResID(attr.DisabledResID)
		self:SetEnable(false)
	end
	--XLMessageBox(self:GetID()..self:GetResID())
end

function DeleteBtn()
end

function OnMouseEnter(self, x, y)
	local attr = self:GetOwnerControl():GetAttribute()
	--local attr = self:GetAttribute()
	local objTree = self:GetOwner()
	local img = self:GetOwnerControl():GetControlObject("btn.img")
	local aniFactory = XLGetObject("Xunlei.UIEngine.AnimationFactory")
	
	--XLMessageBox(attr.AniImagelistID)
	if attr.BtnStatus=="normal" then
		if aniFactory then
			local seqAnim = aniFactory:CreateAnimation("SeqFrameAnimation")
			seqAnim:BindImageObj(img)
			seqAnim:SetResID(attr.AniImagelistID)
			seqAnim:SetTotalTime(500)
			local cookie = seqAnim:AttachListener(true,function (self,oldState,newState)
				end)
			objTree:AddAnimation(seqAnim)
			seqAnim:Resume()
		else
			XLMessageBox("anifailed")
		end
	else
		--XLMessageBox("btndisabled")
	end
end

function OnMouseLeave(self, x, y)
	local attr = self:GetOwnerControl():GetAttribute()
    local objTree = self:GetOwner()
	local img = self:GetOwnerControl():GetControlObject("btn.img")
	--local left, top, right, bottom = image:GetObjPos()
	local aniFactory = XLGetObject("Xunlei.UIEngine.AnimationFactory")
	if attr.BtnStatus=="normal" then
		if aniFactory then
			local seqAnim = aniFactory:CreateAnimation("SeqFrameAnimation")
			seqAnim:BindImageObj(img)
			seqAnim:SetResID(attr.AniImagelistID)
			seqAnim:SetTotalTime(500)
			seqAnim:SetReverse(true)
			local cookie = seqAnim:AttachListener(true,function (self,oldState,newState)
				end)
			objTree:AddAnimation(seqAnim)
			seqAnim:Resume()
		end
	end
	return 0,true,true
end
function OnLButtonDown(self)
	local owner = self:GetOwnerControl()
	local enable = owner:GetEnable()
	if enable then
		self:SetTextureID("toolbar.btn.bkg.down")
		self:SetCaptureMouse(true)
	end
end
function OnLButtonUp(self, x, y)
	local left, top, right, bottom = self:GetObjPos()
	local owner = self:GetOwnerControl()
	if owner:GetEnable() then
		if x >= 0 and x < (right - left) and y >= 0 and y < (bottom - top) then
			owner:ChangeStatus("hover")
			owner:FireExtEvent("OnClick")
		else
			owner:ChangeStatus("normal")
		end
		self:SetCaptureMouse(false)
	end
end

--用来定义各按钮的处理函数
local function OnBtnNewTaskClick(self)--新建任务
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	local path, name, torrent_type, files = coolClientProxy:SelectTorrent()
	
	if path == -1 then
		--不是种子文件
		XLMessageBox("path  = -1")
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

local function OnBtnDeleteClick(self, index)--删除任务

	local templateManager = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local mainWnd = hostWndManager:GetHostWnd("MainFrame")
	local modalHostWndTemplate = templateManager:GetTemplate("Thunder.MessageBox","HostWndTemplate")
	local modalHostWnd = modalHostWndTemplate:CreateInstance("Thunder.MessageBox.Instance")
	local objectTreeTemplate = templateManager:GetTemplate("Thunder.MessageBox","ObjectTreeTemplate")
	local uiObjectTree = objectTreeTemplate:CreateInstance("Thunder.MessageBox.Instance")
	modalHostWnd:BindUIObjectTree(uiObjectTree)
	
	local userData = {Title = "删除", Icon="bitmap.confirmmodal.warning", Content="删除提示:确定要删除这个任务么？", 
		Object = self:GetOwnerControl(), EventName = "OnDeleteTaskConfirm"}
	modalHostWnd:SetUserData(userData)
	modalHostWnd:DoModal(mainWnd:GetWndHandle())
	
	local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")	
	objtreeManager:DestroyTree("Thunder.MessageBox.Instance")
	hostWndManager:RemoveHostWnd("Thunder.MessageBox.Instance")

	--XLMessageBox(#attr.ItemDataTable.." cursel:"..listbox:GetCurSel())
end

local function OnBtnSeedClick(self)--制作种子的对话框
	local templateManager = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local mainWnd = hostWndManager:GetHostWnd("MainFrame")
	local modalHostWndTemplate = templateManager:GetTemplate("Thunder.SeedModal","HostWndTemplate")
	local modalHostWnd = modalHostWndTemplate:CreateInstance("Thunder.SeedModal.Instance")
	local objectTreeTemplate = templateManager:GetTemplate("Thunder.SeedModal","ObjectTreeTemplate")
	local uiObjectTree = objectTreeTemplate:CreateInstance("Thunder.SeedModal.Instance")
	modalHostWnd:BindUIObjectTree(uiObjectTree)
	--XLMessageBox(self:GetOwnerControl():GetOwnerControl():GetClass())Mydownloadpage
	local userData = { Object = self:GetOwnerControl():GetOwnerControl(), EventName = "OnSeedConfrim"}
	modalHostWnd:SetUserData(userData)
	modalHostWnd:DoModal(mainWnd:GetWndHandle())
	
	local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")	
	objtreeManager:DestroyTree("Thunder.SeedModal.Instance")
	hostWndManager:RemoveHostWnd("Thunder.SeedModal.Instance")
end
function OnSeedConfrim(self,eventName, userData)

end

function OnDeleteTaskConfirm(self)
	local listbox = self:GetOwnerControl():GetControlObject("listbox")
	local attr = listbox:GetAttribute()
	listbox:DeleteString(listbox:GetCurSel())
	listbox:UpdateUI()
end

local function OnBtnConfigClick(self)--进入设置
	local header = self:GetOwner():GetUIObject("tabHeader")
	local tabbkg = self:GetOwner():GetUIObject("tabbkg")
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local hostwnd = hostwndManager:GetHostWnd("MainFrame")
	local userData = hostwnd:GetUserData()
	if userData.ConfigPage == false then
		header:AddTabItem("ConfigPage","配置中心","bitmap.tab.config", false)
		tabbkg:AddPage("ConfigPage","ConfigPage")
	end	 
	header:SetActiveTab("ConfigPage", true)
	AsynCall(function() userData.ConfigPage = true end)
end

function OnBtnClick(self)	
	self:ChangeStatus("normal")
	local id = self:GetID()
	if id == "btn.config" then
		OnBtnConfigClick(self)
	elseif id == "btn.newtask" then
		OnBtnNewTaskClick(self)
	elseif id == "btn.delete" then
		OnBtnDeleteClick(self)
	elseif id == "btn.seed" then
		OnBtnSeedClick(self)
	end
end

--改变按钮状态，和改变按钮图片资源同时响应
function UpdateUI(self)
	local attr = self:GetAttribute()
	local img = self:GetControlObject("btn.img")
	local ctrl = self:GetControlObject("ctrl")
	if attr.BtnStatus == "normal" then
		img:SetResID(attr.NormalResID)
		ctrl:SetTextureID("toolbar.btn.bkg.hover")
	elseif attr.BtnStatus == "disabled" then
		img:SetResID(attr.DisabledResID)
	elseif attr.BtnStatus == "hover" then
		
	end
end

function ChangeStatus(self, newStatus)
	local attr = self:GetAttribute()
	attr.BtnStatus = newStatus
	self:UpdateUI()
end

function OnBtnEnableChange(self, isEnable)
	--XLMessageBox("enablechanged")
	if isEnable == true then
		self:ChangeStatus("normal")
	else
		self:ChangeStatus("disabled")
	end
end
