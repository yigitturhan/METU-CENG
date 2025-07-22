
from backend.widgets.sysstat import SysStat
from backend.widgets.filewatch import FileWatch
from backend.widgets.dbquery import DBQuery
from backend.widgets.url_getter import URLGetter
from backend.widgets.message_rotate import MessageRotate
from backend.widgets.timer import Timer
from backend.widgets.chat import Chat
from backend.widgets.dbupdate import DBUpdate
from backend.widgets.fileshare import FileShare


COMPONENT_REGISTRY = {
    "SysStat": SysStat,
    "FileWatch": FileWatch,
    "DBQuery": DBQuery,
    "URLGetter": URLGetter,
    "MessageRotate": MessageRotate,
    "Timer": Timer,
    "Chat": Chat,
    "DBUpdate": DBUpdate,
    "FileShare": FileShare,
}
