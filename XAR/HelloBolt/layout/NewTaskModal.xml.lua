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

local maskani = nil
local turnoffani = nil
function OnOKClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local userData = hostwnd:GetUserData()
	local savepath = owner:GetUIObject("savepath"):GetText()
	local torrentpath = userData.path
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')

	local ret = coolClientProxy:AddNewDownload(torrentpath, savepath, userData.SelectedFiles)
	if ret == -1 then
		--添加下载失败
		hostwnd:EndDialog(0)
		return
	else
		hostwnd:EndDialog(0)
		local treeManager = XLGetObject("Xunlei.UIEngine.TreeManager")                    
		local tree = treeManager:GetUIObjectTree("MainObjectTree")
		local tabctrl= tree:GetUIObject("tabbkg")
		tabctrl:ActivePage("MydownloadPage")
		return
	end
	local treeManager = XLGetObject("Xunlei.UIEngine.TreeManager")				
	local tree = treeManager:GetUIObjectTree("MainObjectTree")
	tree:GetUIObject("tabbkg"):GetControlObject("MydownloadPage"):UpdateListBox()
	
	local root = owner:GetRootObject()
	local left,top,right,bottom = root:GetObjPos()
	local height = bottom - top
	
	local xlgraphic = XLGetObject("Xunlei.XLGraphic.Factory.Object")
	local theBitmap = xlgraphic:CreateBitmap("ARGB32",452,500)
	local render = XLGetObject("Xunlei.UIEngine.RenderFactory")
	local maskobj = owner:GetUIObject("mask")
	local turnoffobj = owner:GetUIObject("turnoff")
	render:RenderObject(root,theBitmap)
	root:SetVisible(false,true)
	if not turnoffani then
		local ani = XLGetObject("Xunlei.UIEngine.AnimationFactory"):CreateAnimation("TurnObjectAnimation")
		turnoffobj:SetVisible(true,true)
		local obj1 = turnoffobj:GetChildByIndex(0)
		obj1:SetBitmap(theBitmap)
		local obj2 = nil
		if obj1:GetVisible() then
			obj2 = turnoffobj:GetChildByIndex(1)
			ani:SetFlag("RoundX")
			ani:SetFlag("Widdershins")
		else
			obj2 = obj1
			obj1 = turnoffobj:GetChildByIndex(1)
			ani:SetFlag("RoundY")
			ani:SetFlag("Deasil")
			--ani:SetCentrePoint(20,20)
		end
		
		ani:SetBlendMode("AntiAlias")
		ani:BindRenderObj(obj1, obj2)
		ani:SetTotalTime(500)
		turnoffobj:GetOwner():AddAnimation(ani)
		ani:AttachListener(true, 
			function(ani,state1,state2) 
				if state2 == 4 then turnoffani = nil 
				hostwnd:EndDialog(0)
			end 
			end)
		ani:Resume()
		turnoffani = ani
	end
	
	--[[
	if not maskani then
		local obj1 = maskobj
		obj1:SetVisible(true,true)
		local obj2 = obj1:GetChildByIndex(0)
		local ani = XLGetObject("Xunlei.UIEngine.AnimationFactory"):CreateAnimation("MaskChangeAnimation")
		local maskorgx,maskorgy,masksizex,masksizey = obj1:GetMaskPosition()
		local left,top,right,bottom = obj1:GetObjPos()
		local width = right - left
		local height = bottom - top
		if maskorgy == 0 then
			ani:SetMaskKeyFrame(0,0,masksizex,masksizey,0,height - masksizey,masksizex,masksizey)
		else
			ani:SetMaskKeyFrame(0,height - masksizey,masksizex,masksizey,0,0,masksizex,masksizey)
		end	
		ani:SetTotalTime(1000)
		ani:BindMaskObj(obj1)
		maskobj:GetOwner():AddAnimation(ani)
			ani:AttachListener(true, 
				function(ani,state1,state2) 
					if state2 == 4 then  
						maskani = nil 
						hostwnd:EndDialog(0)
					end 
				end)
		ani:Resume()
		maskani = ani
	end
	]]
end

function OnChooseAllClick(self)
	local owner = self:GetOwner()
	local listbox = owner:GetUIObject("listbox")
	local attr = listbox:GetAttribute()
	--XLMessageBox(#attr.ItemDataTable.."InstanNum:"..#attr.InstanceTable)
	for i=1,#attr.InstanceTable do
		local checkbox = listbox:GetItemByIndex(i):GetControlObject("check")
		if checkbox:GetCheck() == false then
			checkbox:SetCheck(true)
		end
	end
end

function OnChooseOpClick(self)
	local owner = self:GetOwner()
	local listbox = owner:GetUIObject("listbox")
	local attr = listbox:GetAttribute()
	--XLMessageBox(#attr.ItemDataTable.."InstanNum:"..#attr.InstanceTable)
	for i=1,#attr.InstanceTable do
		local checkbox = listbox:GetItemByIndex(i):GetControlObject("check")
		checkbox:SetCheck(not (checkbox:GetCheck()))
	end
end