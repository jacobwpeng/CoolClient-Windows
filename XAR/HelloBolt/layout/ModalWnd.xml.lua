--创建窗口时调用
--目前用来居中显示
function OnCreate(self)
	self:Center()
end

--确认任务创建
local function AlphaAni(self)	
	local tmp = self:GetAlpha()

	if tmp > 0 then
		self:SetAlpha(tmp-15,true)
	end
	
end
function OnNewTaskDlgOKClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	
	local layout = owner:GetUIObject("newtaskdlg.root")
	
	local counter1 = SetTimer(function() AlphaAni(layout) end, 29)
	local counter2 = SetOnceTimer(function() hostwnd:EndDialog(0) end, 520)
	local counter3 = SetOnceTimer(function() KillTimer(counter1) end, 520)
end

function OnNewTaskDlgCancelClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(0)
end

--控制两个按钮效果的函数
function OnNewTaskDlgButtonEnter(self)
	self:SetTextureID("texture.dlg.button_hover")
end
function OnNewTaskDlgButtonLeave(self)
	self:SetTextureID("texture.dlg.button_normal")
end

--编辑框的发光效果
function OnNewTaskDlgEditFocusChange(self, isFocus)
	local parent = self:GetParent()
	if isFocus then
		parent:SetTextureID("texture.edit.light.bkg")
	end
end